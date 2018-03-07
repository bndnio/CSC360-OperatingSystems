#include <fstream>
#include "train.cpp"
#include "station.cpp"
#include "dispatcher.cpp"

int main(int argc, char *argv[])
{
    // Ensure input has correct number of params
    if (argc != 2)
    {
        std::cout << "Incorrect number of params" << std::endl;
    }

    // Various
    long int startTime;

    // Output magement
    pthread_mutex_t coutMutex;
    pthread_mutex_init(&coutMutex, NULL);
    pthread_cond_t coutCond;
    pthread_cond_init(&coutCond, NULL);

    // ----- Setup Stations ------
    Station westStation, eastStation;
    westStation.trainQueue[100];
    pthread_mutex_init(&westStation.trainQueueMutex, NULL);
    westStation.stationInput = NULL;
    pthread_cond_init(&westStation.inputSignal, NULL);
    pthread_cond_init(&westStation.inputEmpty, NULL);
    eastStation.trainQueue[100];
    pthread_mutex_init(&eastStation.trainQueueMutex, NULL);
    eastStation.stationInput = NULL;
    pthread_cond_init(&eastStation.inputSignal, NULL);
    pthread_cond_init(&eastStation.inputEmpty, NULL);
    Stations stations;
    stations.east = &eastStation;
    stations.west = &westStation;

    // ----- Setup Trains ------
    // Start signal
    bool startLoadingSignal = false;
    // Create file stream and string to stream to
    std::ifstream infile(argv[1]);
    std::string trainEntry;
    // Parse in file
    int trainNum = 1;
    while (std::getline(infile, trainEntry))
    {
        TrainThreadArgs *nextTrainThreadArgs = newTrainThreadArgs();

        nextTrainThreadArgs->numberInput = trainNum;
        nextTrainThreadArgs->startSignal = &startLoadingSignal;
        nextTrainThreadArgs->coutMutex = &coutMutex;
        nextTrainThreadArgs->coutCond = &coutCond;
        nextTrainThreadArgs->stations = &stations;
        nextTrainThreadArgs->startTime = &startTime;

        std::istringstream iss(trainEntry);
        iss >> nextTrainThreadArgs->travelInput >> nextTrainThreadArgs->loadTimeInput >> nextTrainThreadArgs->crossTimeInput;

        pthread_mutex_lock(&coutMutex);
        std::cout << "Reading in: " << nextTrainThreadArgs->travelInput << " " << nextTrainThreadArgs->loadTimeInput << " " << nextTrainThreadArgs->crossTimeInput << std::endl;
        pthread_mutex_unlock(&coutMutex);

        pthread_t some_thread;
        pthread_create(&some_thread, NULL, &createTrain, (void *)nextTrainThreadArgs);

        trainNum += 1;
    }

    // Sleep main thread for 1 second to ensure threads finish prepping
    struct timespec tim, tim2;
    tim.tv_sec = 1;
    tim.tv_nsec = 0;
    nanosleep(&tim, &tim2);

    struct timeval tp;
    gettimeofday(&tp, NULL);
    startTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    // Tell threads to start loading
    startLoadingSignal = true;

    while (true)
        ;

    return 0;
};
