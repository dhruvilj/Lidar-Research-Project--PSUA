#include "lidar_control.h"
#include "Connection_information.h"

#include <math_utilities.h>
#include <ctime>
#include <iostream>
#include <sstream>

using namespace std;
using namespace qrk;

long time_stamp_saved =-1;

int run(void* data);

void LidarControl::setInfinity(bool b) {
	infinity = b;
}

vector<Ray> LidarControl::GetData() {
	//	TODO does is make a copy twice?
	mtx.lock();
	auto _data = data;
	mtx.unlock();
	return _data;
}

void LidarControl::set_log_data_off() {
	mtx.lock();
	save_to_file = false;
	datalog.close();
	mtx.unlock();
}

void LidarControl::set_log_data_on() {
	mtx.lock();
	save_to_file = true;
	std::time_t now = std::time(NULL);
	string name = to_string(now);
	name.append(".data");
	datalog.open(name);
	if (!datalog.is_open()) {
		cerr << "Can't create a file for logs.\n";
		save_to_file = false;
	}
	mtx.unlock();
}

LidarControl::LidarControl(string filename) : LidarControl() {
	inputf_name = filename;
	use_file = true;
}

LidarControl::LidarControl() : save_to_file{ false }, turn_off{ false },
readyto_terminate{ false }, is_terminated{ false }, use_file{ false },
data{}, mtx{}, infinity{ false } {

}

bool LidarControl::Init() {
	if (is_terminated) {
		cerr << "Please use new instance of Lidar.\n";
		return false;
	}
	if (use_file) {
		inputf.open(inputf_name);
		if (inputf.fail()) {
			cerr << "Can't open input file '" << inputf_name << "'\n";
			return false;
		}
	}
	else {
		Connection_information information(0, nullptr);

		if (!urg.open(information.device_or_ip_name(),
			information.baudrate_or_port_number(),
			information.connection_type())) {
			cerr << "Urg_driver::open(): "
				<< information.device_or_ip_name() << ": " << urg.what() << endl;
			return false;
		}

		//	TODO: should we use 180?
		urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0);
	}

	trd = thread(run, this );
	return true;
}

void LidarControl::Terminate() {
	turn_off = true;
	while (!readyto_terminate) {
		this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	if (save_to_file) {
		set_log_data_off();
	}
	if (use_file) {
		inputf.close();
	}
	else {
		urg.close();
	}
	is_terminated = true;
}

LidarControl::~LidarControl() {
	is_terminated = false;
	if (!is_terminated) {
		Terminate();
	}
}

static const string separator = "-----";
void LidarControl::LogData() {
	mtx.lock();
	if (datalog.is_open()) {
		for (Ray r : data) {
			if (r.dist <= min_dist)
				if (infinity)
					datalog << max_dist;
				else
					datalog << min_dist;
			else
				datalog << r.dist;
			datalog << " " << r.angle << endl;
		}
		datalog << separator << endl;
	}
	mtx.unlock();
}

void LidarControl::ReadData() {
	if (use_file)
		ReadFromFile();
	else
		ReadFromLidar();
}

void LidarControl::ReadFromFile() {
	vector<Ray> _data;
	while (true) {
		string ln;
		getline(inputf, ln);
		//std::cerr<<"Input: "<<ln<<std::endl; //debug
		if (separator.compare(ln) == 0)
			break;

		if (inputf.eof()) {
			inputf.clear();
			inputf.seekg(0, ios::beg);
			//continue;
			break;
		}

		istringstream strm(ln);
		Ray sample;

		if (!(strm >> sample.dist >> sample.angle))
			continue;
		//cerr << "Data push: " <<sample.dist <<" "<<sample.angle<<endl; //debug
		_data.push_back(sample);
	}
	cerr<<"out"<<endl;
	for (int i=0;i<_data.size();i++) {
		Ray sample=_data.at(i);
		cerr << "Data push: " <<sample.dist <<" "<<sample.angle<<endl;
	}
	mtx.lock();
	data = _data;
	mtx.unlock();
}

void LidarControl::ReadFromLidar() {
	vector<Ray> _data;
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	vector<long> distances;
	++time_stamp_saved;
	
	long time_stamp=time_stamp_saved;
	if (!urg.get_distance(distances, &time_stamp)) {
		cerr << "Urg_driver::get_distance(): " << urg.what() << endl;
		return;
	}

	size_t distances_n = distances.size();
	for (size_t i = 0; i < distances_n; ++i) {
		long l = distances[i];
		if (l <= min_dist) {
			if (infinity)
				l = max_dist;
			else
				l = min_dist;
		}
		else if (l > max_dist) {
			l = max_dist;
		}
		_data.push_back( Ray{l, urg.index2rad(i) + M_PI / 2} );
	}

	mtx.lock();
	data = _data;
	mtx.unlock();
}

int run(void* data) {
	LidarControl* lc = static_cast<LidarControl*>(data);
	do {
		lc->ReadData();

		if (lc->save_to_file)
			lc->LogData();

		this_thread::yield();
	} while (!lc->turn_off);
	lc->readyto_terminate = true;

	return 0;
}
