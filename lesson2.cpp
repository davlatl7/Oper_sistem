#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <windows.h>


using namespace std;
using namespace std::chrono;


string current_time() {
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    stringstream ss;
    ss << put_time(localtime(&t), "%H:%M:%S")
       << "." << setw(3) << setfill('0') << ms.count();
    return ss.str();
}

vector<string> stations = {
    "Душанбе",
    "Вахш",
    "Бохтар",
    "Куляб"
};


vector<mutex> station_mutex(stations.size());

mutex cout_mutex;

void log_event(int train_id, const string& msg) {
    lock_guard<mutex> lock(cout_mutex);
    cout << "[" << current_time() << "] "
         << "Поезд #" << train_id << ": " << msg << endl;
}


void train_thread(int train_id, int start_station) {
    for (size_t i = start_station; i < stations.size(); ++i) {

        log_event(train_id, "ожидает станцию " + stations[i]);
        station_mutex[i].lock();

        log_event(train_id, "прибыл на станцию " + stations[i]);
        this_thread::sleep_for(seconds(2));

        log_event(train_id, "отправляется со станции " + stations[i]);
        station_mutex[i].unlock();

        if (i + 1 < stations.size()) {
            log_event(train_id,
                      "следует к станции " + stations[i + 1]);
            this_thread::sleep_for(seconds(3));
        }
    }

    log_event(train_id, "завершил маршрут");
}


int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    const int TRAIN_COUNT = 8;
    vector<thread> trains;

    for (int i = 0; i < TRAIN_COUNT; ++i) {
        int start_station = i % stations.size();
        trains.emplace_back(train_thread, i + 1, start_station);
    }

    for (auto& t : trains) {
        t.join();
    }

    cout << "Все поезда завершили движение." << endl;
    return 0;
}

