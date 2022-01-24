#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <cmath>
#include <map>

class OrderBook
{
    public:

        //FUNCTIONS IMPLEMENTED BY THE INSTRUCTOR

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

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);


        //FUNCTIONS I HAVE WRITTEN

        //Similar to getNextTime, but returns "end" if the there is no more further time rather than looping
        std::string getNextTimeWithoutLoop(std::string timestamp); 

        //Checks how many previous (including the current) timestamps are available and returns the number
        int getPastTimes(std::string timestamp); 

        //Returns a vector of closest past timestamps
        std::vector<std::string> getVectorOfPreviousSteps(int step, std::string timestamp); 

        //Returns a vector of current and all past timestamps
        std::vector<std::string> getVectorOfAllPreviousAndCurrentSteps(std::string timestamp); 

        //Calculates average price for asks/bids in input timestamps
        double calculateAveragePrice(std::vector<std::string> timestamps,std::string product, std::string orderBooktype); 
        //Similar to calculateAveragePrice, but only calculates for a given timestep
        double calculateOneAveragePrice(std::string timestamp, std::string product, std::string orderBooktype); 

        //Calculates the moving average from the beginning to the current time
        double calculateMovingAverage(std::string timestamp, std::string product, std::string orderBooktype); 
        //Predicts the next price according to the input by calculating from the first timestamp until the currentTime
        double predictPriceDifference(std::vector<std::string> timestamps, std::string product, std::string orderBooktype, std::string maxormin); 

        //Calculates the change in price as percentage between an input of previous timesteps and the current time
        double calculateChangePrice(int step, std::string currentTime, std::string product, std::string orderBooktype); 
        //Returns the previous timestamp according to how much step before the input asked
        std::string getReferencedPreviousTimestamp(int step, std::string timestamp); 





    private:
        //Raw orders vector
        std::vector<OrderBookEntry> orders;

        //orders vector mapped by timestamps
        std::map< std::string, std::vector<OrderBookEntry> > mappedOrders;

        //FUNCTIONS I HAVE WRITTEN
        
        //return vector of all know products in the dataset, works the same way with getKnownProducts but returns a vector of timestamps
        std::vector<std::string> getKnownTimestamps();
        
        //maps the orders vector by timestamps
        void mapOrdersByTimestamp();



};

