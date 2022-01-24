#include "OrderBook.h"
#include "CSVReader.h"
#include <algorithm>
#include <iostream>


/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    //Creating the orders vector
    orders = CSVReader::readCSV(filename);

    //Map the orders vector by timestamps
    mapOrdersByTimestamp();
}

/** return vector of all known products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string, bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }

    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}

/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
    std::string product,
    std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : mappedOrders[timestamp])
    {
        if (e.orderType == type &&
            e.product == product)
        {
            orders_sub.push_back(e);
        }
    }
    return orders_sub;
}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    //return "Example timestamp";
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

//Similar to getNextTime, but returns "end" if the there is no more further time rather than looping
std::string OrderBook::getNextTimeWithoutLoop(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        return "end";
    }
    return next_timestamp;
}



//Checks how many previous (including the current) timestamps are available and returns the number
int OrderBook::getPastTimes(std::string timestamp)
{
    int timeCounter = 0; 
    std::vector<std::string> timestamps;
    bool uniqueTimestamp = true;

    //Checks if the timestamp is lower than the input timestamp. Also checks if the timestamp appeared for the first time.
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp <= timestamp)
        {
            for (std::string p : timestamps)
            {
                if ( p == e.timestamp)
                {
                    uniqueTimestamp = false;
                }
            }

            if (uniqueTimestamp)
            {
                timeCounter++;
                timestamps.push_back(e.timestamp);
            }

            uniqueTimestamp = true;

        }
    }

    return timeCounter;
}

//Returns a vector of closest past time steps
std::vector<std::string> OrderBook::getVectorOfPreviousSteps(int step, std::string timestamp)
{
    std::vector<std::string> timestamps; //Will be used to collect all timestamps equal to and lower than the input timestamp
    bool uniqueTimestamp = true;

    //Checks if the timestamp is lower than the input timestamp. Also checks if the timestamp appeared for the first time.
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp <= timestamp)
        {
            for (std::string p : timestamps)
            {
                if (p == e.timestamp)
                {
                    uniqueTimestamp = false;
                }
            }

            if (uniqueTimestamp)
            {
                timestamps.push_back(e.timestamp);
            }

            uniqueTimestamp = true;

        }
    } //Vector containing timestams equal to and lower than the input time stamp is ready.


    //Slicing the timestamps much earlier than the asked input
    int slice_start = timestamps.size() - step - 1;
    std::vector<std::string> returningTimestamps;
    
    for (int i = slice_start; i < timestamps.size(); ++i)
    {
        returningTimestamps.push_back(timestamps[i]);
    }

    return returningTimestamps;
}

//Returns a vector of closest past time steps
std::vector<std::string> OrderBook::getVectorOfAllPreviousAndCurrentSteps(std::string timestamp)
{
    std::vector<std::string> timestamps; //Will be used to collect all timestamps equal to and lower than the input timestamp
    bool uniqueTimestamp = true;

    //Checks if the timestamp is lower than the input timestamp. Also checks if the timestamp appeared for the first time.
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp < timestamp)
        {
            for (std::string p : timestamps)
            {
                if (p == e.timestamp)
                {
                    uniqueTimestamp = false;
                }
            }

            if (uniqueTimestamp)
            {
                timestamps.push_back(e.timestamp);
            }

            uniqueTimestamp = true;

        }
    } //Vector containing timestams equal to and lower than the input time stamp is ready.

    return timestamps;
}


//Calculates average price for asks/bids in input timestamps. Used for "avg" command.
double OrderBook::calculateAveragePrice(std::vector<std::string> timestamps, std::string product, std::string orderBooktype)
{
    double average = 0;
    int counter = 0;

    if (orderBooktype == "ask")
    {
        for (OrderBookEntry& e : orders)
        {
            for (std::string p : timestamps)
            {
                if (e.timestamp == p && e.product == product && e.orderType == OrderBookType::ask)
                {
                    average = average + e.price;
                    counter++;
                }
            }
        }
    }

    if (orderBooktype == "bid")
    {
        for (OrderBookEntry& e : orders)
        {
            for (std::string p : timestamps)
            {
                if (e.timestamp == p && e.product == product && e.orderType == OrderBookType::bid)
                {
                    average = average + e.price;
                    counter++;
                }
            }
        }
    }

    return (average / counter);
}

//Similar to calculateAveragePrice, but only calculates for a given timestep. Used for "predict" command.
double OrderBook::calculateOneAveragePrice(std::string timestamp, std::string product, std::string orderBooktype)
{
    double total = 0;
    int counter = 0;

    if (orderBooktype == "ask")
    {
        for (OrderBookEntry& e : orders)
        {
            if (e.timestamp == timestamp && e.product == product && e.orderType == OrderBookType::ask)
            {
                total = total + e.price;
                counter++;
            }
            
        }
    }

    if (orderBooktype == "bid")
    {
        for (OrderBookEntry& e : orders)
        {

            if (e.timestamp == timestamp && e.product == product && e.orderType == OrderBookType::bid)
            {
                total = total + e.price;
                counter++;

            }
        }
    }

    return (total / counter);
}

//Predicts the future step price using Exponential Moving Average (EMA) formula.
double OrderBook::calculateMovingAverage(std::string timestamp, std::string product, std::string orderBooktype)
{
    double EMA = 0; //Exponential Moving Average, will return this.

    //Other variables to calculate EMA
    double dailyAverage = 0; 
    double multiplier = 0;
    float stepCounter = 0;

    std::string orderTime = getEarliestTime(); // Starting from the earliest time
    
    std::cout << "currentTime is: " << orderTime << std::endl;

        while (orderTime <= timestamp && orderTime != "end") //Works until the given time (aka current time for advisor bot)
        {
            dailyAverage = calculateOneAveragePrice(orderTime, product, orderBooktype); //Calculated the Average of the given timestep
            
            orderTime = getNextTimeWithoutLoop(orderTime); //Updated the order time, get to the next timestep

            stepCounter++; //Updated the step counter so that new multiplier will be calculated

            multiplier = (2 / (stepCounter + 1)); //Updated the multiplier

            if (!std::isnan(dailyAverage) ) // If there is no dailyAverage (no trade requests happened in the curent time) EMA will not be updated.
            {
                EMA = dailyAverage * multiplier + EMA * (1 - multiplier); //Updated the EMA using the formula
            }

        }

    return EMA;
}

//Predicts the next price according to the input by calculating from the first timestamp until the currentTime
double OrderBook::predictPriceDifference(std::vector<std::string> timestamps, std::string product, std::string orderBooktype, std::string maxormin)
{
    double tempPrice = 0;
    double totalDifference = 0;
    double tempAvg = 0;
    double tempDiff = 0;
    float stepCounter = 0;

    if (maxormin == "max")
    {
        if (orderBooktype == "ask")
        {
            for (std::string& v : timestamps)
            {
                for (OrderBookEntry& e : orders)
                {
                    if (e.timestamp == v && e.product == product && e.orderType == OrderBookType::ask && e.price > tempPrice)
                    {
                        tempPrice = e.price; //max price is updated
                    }
                }

                if (tempPrice != 0) // If tempPrice == 0 (no trade requests happened in the curent time) , other variables will not be updated.
                {
                    tempAvg = calculateOneAveragePrice(v, product, orderBooktype);
                    tempDiff = tempPrice - tempAvg;

                    totalDifference = totalDifference + tempDiff; //difference is added to the total Difference is added
                    stepCounter++; //Counter is incremented by 1 in order to calcualte average

                    tempPrice = 0; // Resetting the tempPrice
                }

            }
        }

        if (orderBooktype == "bid")
        {
            for (std::string& v : timestamps)
            {
                for (OrderBookEntry& e : orders)
                {
                    if (e.timestamp == v && e.product == product && e.orderType == OrderBookType::bid && e.price > tempPrice)
                    {
                        tempPrice = e.price; //max price is updated
                    }
                }

                if (tempPrice != 0) // If tempPrice == 0 (no trade requests happened in the curent time) , other variables will not be updated.
                {
                tempAvg = calculateOneAveragePrice(v, product, orderBooktype);
                tempDiff = tempPrice - tempAvg;

                totalDifference = totalDifference + tempDiff; //difference is added to the total Difference is added
                stepCounter++; //Counter is incremented by 1 in order to calcualte average

                tempPrice = 0; // Resetting the tempPrice
                }
            }
        }

    }

    if (maxormin == "min")
    {
        double tempPrice = 99999999999; //temp price is updated to a very big number so that the first valid order will be smaller
        
        if (orderBooktype == "ask")
        {
            for (std::string& v : timestamps)
            {
                for (OrderBookEntry& e : orders)
                {
                    if (e.timestamp == v && e.product == product && e.orderType == OrderBookType::ask && e.price < tempPrice)
                    {
                        tempPrice = e.price; //min price is updated
                    }
                }

                if (tempPrice != 0) // If tempPrice == 0 (no trade requests happened in the curent time) , other variables will not be updated.
                {
                    tempAvg = calculateOneAveragePrice(v, product, orderBooktype);
                    tempDiff = tempPrice - tempAvg;

                    totalDifference = totalDifference + tempDiff; //difference is added to the total Difference is added
                    stepCounter++; //Counter is incremented by 1 in order to calcualte average

                    tempPrice = 0; // Resetting the tempPrice
                }
            }
        }

        if (orderBooktype == "bid")
        {
            for (std::string& v : timestamps)
            {
                for (OrderBookEntry& e : orders)
                {
                    if (e.timestamp == v && e.product == product && e.orderType == OrderBookType::bid && e.price < tempPrice)
                    {
                        tempPrice = e.price; //min price is updated
                    }
                }

                if (tempPrice != 0) // If tempPrice == 0 (no trade requests happened in the curent time) , other variables will not be updated.
                {
                    tempAvg = calculateOneAveragePrice(v, product, orderBooktype);
                    tempDiff = tempPrice - tempAvg;

                    totalDifference = totalDifference + tempDiff; //difference is added to the total Difference is added
                    stepCounter++; //Counter is incremented by 1 in order to calcualte average

                    tempPrice = 0; // Resetting the tempPrice
                }
            }
        }

    }

    return (totalDifference / stepCounter); //returns the average difference from the beginning to the given time (aka current time for advisor bot)
}

std::string OrderBook::getReferencedPreviousTimestamp(int step, std::string timestamp)
{
    //Gets the number of total timestamps until the current time
    int TotalTimesteps = getPastTimes(timestamp); 

    int referenceStep = TotalTimesteps - step;
    int timeCounter = 0;

    //Will be used to collect all timestamps equal to and lower than the input timestamp
    std::vector<std::string> timestamps; 
    
    bool uniqueTimestamp = true;

    //Checks if the timestamp is lower than the input timestamp. Also checks if the timestamp appeared for the first time.
    for (OrderBookEntry& e : orders)
    {
        for (std::string p : timestamps)
        {
            if (p == e.timestamp)
            {
                uniqueTimestamp = false;
            }
        }
        if (uniqueTimestamp)
        {
            timeCounter++;
            timestamps.push_back(e.timestamp);
        }
        if (timeCounter == referenceStep)
        {
            break;
        }
        uniqueTimestamp = true;
    }
    return timestamps[referenceStep - 1];
}

//Calculates the change in price as percentage between an input of previous timesteps and the current time
double OrderBook::calculateChangePrice(int step, std::string currentTime, std::string product, std::string orderBooktype)
{
    //The timestamp of the previous time step
    std::string referenceTimestamp = getReferencedPreviousTimestamp(step, currentTime);

    //The current timestamp
    std::string currentTimestamp = currentTime;

    //Calculates the average price of the previous time step
    double referenceAvgPrice = calculateOneAveragePrice(referenceTimestamp, product, orderBooktype);

    //These are used for NaN price errors. referenceTimestamp will be updated, so for better feedback from advisor bot comes with the original one.
    int stepChecker = step;
    int refStepUpdated = step;
    std::string originalReferenceTimestamp = referenceTimestamp; 

    //If there is no price for the indicated previous step, then 1 earlier time step will be checked, until a price is found or no previous time step left.
    //If step checker reaches to zero, then all the previous time steps have been checked but no price has been found
    while (std::isnan(referenceAvgPrice) && stepChecker != 0)
    {
        refStepUpdated++;
        std::string referenceTimestamp = getReferencedPreviousTimestamp(refStepUpdated, currentTime);
        referenceAvgPrice = calculateOneAveragePrice(referenceTimestamp, product, orderBooktype);
        stepChecker--;
    }

    //If no price has been found, then it means, until the indicated timestamp, no bids or asked happened.
    if (stepChecker == 0)
    {
        std::cout << "Vader: There is no " << orderBooktype << " for " << product << " until " << originalReferenceTimestamp << "." << std::endl;

        //Returns a dummy number, will process it in MerkelMain. No bid or ask happened until the referenceTime since the beginning of the orderBook. Price Change cannot be determined.
        return -1000000.9898;  
    }

    //Calculates the average price of the current time step
    double currentAvgPrice = calculateOneAveragePrice(currentTime, product, orderBooktype);

    //The similar checks which have been made for previous time step will be applied to the current time step
    //The difference is, the price will be checked until the previous time step. 
    //If there is a price for previous time step, but no bids or asks happened, then it can be assumed that th eprice is not changed. 

    //Will check until the updated reference step if no bid or ask happened since.
    stepChecker = getPastTimes(currentTime) - (getPastTimes(currentTime) - refStepUpdated); 
    int curStepUpdated = 0;

    while (std::isnan(currentAvgPrice) && stepChecker != 0)
    {
        stepChecker--;
        if (stepChecker == 0)
        {
            //Returns zero as there was a bid or ask since referenceTime but no more until the current time. The price is not changed since as there was no more ask or bid.
            return 0; 
        }

        curStepUpdated++;
        std::string currentTimestamp = getReferencedPreviousTimestamp(curStepUpdated, currentTime);
        currentAvgPrice = calculateOneAveragePrice(currentTimestamp, product, orderBooktype);
    }

    //Calculating the change and rounding it for a better output for humans
    double change = ((currentAvgPrice - referenceAvgPrice) / referenceAvgPrice) * 10000;
    change = std::round(change);
    change = change / 100;

    //If orderBook is in the end, the step is also the maximum and there happened no bid or ask, the previous check for stepChecker != 0 becomes true while change will be NaN. This checks for it.
    if ( (std::isnan(change)) ) 
    {
        return 0; //Returns zero as there was a bid or ask since referenceTime but no more until the current time. The price is not changed since as there was no more ask or bid.
    }

    return change;

}


void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

/** return vector of all known products in the dataset*/
std::vector<std::string> OrderBook::getKnownTimestamps()
{
    std::vector<std::string> timestamps;

    std::map<std::string, bool> timeMap;

    for (OrderBookEntry& e : orders)
    {
        timeMap[e.timestamp] = true;
    }

    // now flatten the map to a vector of strings
    for (auto const& e : timeMap)
    {
        timestamps.push_back(e.first);
    }

    return timestamps;
}

void OrderBook::mapOrdersByTimestamp()
{

    //Temporary vector that will be mapped to a key timestamp
    std::vector<OrderBookEntry> tempOrders;

    //Getting the vector of all timestamps in the order book
    std::vector<std::string> timestamps = getKnownTimestamps();
    std::vector<std::string> products = getKnownProducts();



    for (OrderBookEntry& e : orders)
    {
        if (mappedOrders.find(e.timestamp) == mappedOrders.end())
        {
            //Inserting the empty vector tempOrders into the map
            mappedOrders[e.timestamp] = tempOrders;

            //Inserting the Order Book Entry into the newly created vector
            mappedOrders[e.timestamp].push_back(e);
            
            //Clearing the tempOrders Vector so it will be empty for the next timestamp
            tempOrders.clear();
        }
        else
        {
            //Inserting the Order Book Entry into the vector
            mappedOrders[e.timestamp].push_back(e);
        }

    }
}



std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    // asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask,
        product,
        timestamp);
    // bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid,
        product,
        timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales;

    //Converting vectors of rawAsks and rawBids into maps by timestamp keys

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size() - 1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size() - 1].price << std::endl;

    for (OrderBookEntry& ask : asks)
    {
        //     for bid in bids:
        for (OrderBookEntry& bid : bids)
        {
            //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
                //             sale = new order()
                //             sale.price = ask.price
                OrderBookEntry sale{ ask.price, 0, timestamp,
                    product,
                    OrderBookType::asksale };

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                //             # now work out how much was sold and 
                //             # create new bids and asks covering 
                //             # anything that was not sold
                //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # can do no more with this ask
                    //                 # go onto the next ask
                    //                 break
                    break;
                }
                //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # we adjust the bid in place
                    //                 # so it can be used to process the next ask
                    //                 bid.amount = bid.amount - ask.amount
                    bid.amount = bid.amount - ask.amount;
                    //                 # ask is completely gone, so go to next ask                
                    //                 break
                    break;
                }


                //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount &&
                    bid.amount > 0)
                {
                    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # update the ask
                    //                 # and allow further bids to process the remaining amount
                    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # some ask remains so go to the next bid
                    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;
}

