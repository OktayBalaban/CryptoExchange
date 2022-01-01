#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <cmath>

class OrderBook
{
    public:
        /** construct, reading a csv data file */
        OrderBook(std::string filename);
        /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
        /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type,
            std::string product,
            std::string timestamp);

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the
         * sent time in the orderbook
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);
        std::string getNextTimeWithoutLoop(std::string timestamp); //Similar to getNextTime, but returns "end" if the there is no more further time rather than looping


        int checkPastTimes(std::string timestamp); //Checks how many previous (including the current) timestamps are available and returns the number
        std::vector<std::string> getVectorOfPreviousSteps(int step, std::string timestamp); //Returns a vector of closest past timestamps

        std::vector<std::string> getVectorOfAllPreviousAndCurrentSteps(std::string timestamp); //Returns a vector of current and all past timestamps

        double calculateAveragePrice(std::vector<std::string> timestamps,std::string product, std::string orderBooktype); //Calculates average price for asks/bids in input timestamps
        double calculateOneAveragePrice(std::string timestamp, std::string product, std::string orderBooktype); //Similar to calculateAveragePrice, but only calculates for a given timestep

        double calculateMovingAverage(std::string timestamp, std::string product, std::string orderBooktype); //Calculates the moving average from the beginning to the current time
        double predictPriceDifference(std::vector<std::string> timestamps, std::string product, std::string orderBooktype, std::string maxormin); //Predicts the next price according to input

        double calculateChangePrice(int step, std::string currentTime, std::string product, std::string orderBooktype);  //Calculates the change in price as percentage between an input of previous timesteps and the current time
        std::string getReferencedPreviousTimestamp(int step, std::string timestamp);  //Returns the previous timestamp according to how much step before the input asked

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);

    private:
        std::vector<OrderBookEntry> orders;


};

