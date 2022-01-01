#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "AdvisorBot.h"
#include <fstream>
#include <cctype>
#include <typeinfo>

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}




void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;
    // 7 advisor bot
    std::cout << "7: Summon Vader (Advisor Bot) " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}



void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
            p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}



void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }

    }

    currentTime = orderBook.getNextTime(currentTime);
}


int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-7" << std::endl;
    std::getline(std::cin, line);
    try {
        userOption = std::stoi(line);
    }
    catch (const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-7" << std::endl;
    }
    if (userOption == 1)
    {
        printHelp();
    }
    if (userOption == 2)
    {
        printMarketStats();
    }
    if (userOption == 3)
    {
        enterAsk();
    }
    if (userOption == 4)
    {
        enterBid();
    }
    if (userOption == 5)
    {
        printWallet();
    }
    if (userOption == 6)
    {
        gotoNextTimeframe();
    }
    if (userOption == 7) // Initiates Advisor Bot
    {
        manageAdvisorBot();
    }
}

void MerkelMain::manageAdvisorBot()
{
    std::string userEntry; //Input from the user
    std::string escapeEntry = "May Elon be with you"; //The entry in order to close advisor Bot

    std::cout << "(In order to close Vader, write '" << escapeEntry << "')" << std::endl;
    std::cout << "Vader: What is thy bidding, my master?" << std::endl;



    while (true)
    {
        std::getline(std::cin, userEntry);

        //If the user enters the escape entry, then the bot stops
        if (userEntry == escapeEntry)
        {
            std::cout << "Vader: This is the end for you, my master!" << std::endl;
            break;
        }

        userInput(userEntry);
    }
}


//Main function to run the advisor bot
void MerkelMain::userInput(std::string input)
{
	inputProcess(input);

}


//Funtion to tokenise user input in order to process the input
std::vector<std::string> MerkelMain::inputTokeniser(std::string inputString)
{
	std::vector<std::string> tokenisedInput;
	signed int start, end;
	std::string token;

	start = inputString.find_first_not_of(' ', 0);
	do {
		end = inputString.find_first_of(' ', start);
		if (start == inputString.length() || start == end) break;
		if (end >= 0) token = inputString.substr(start, end - start);
		else token = inputString.substr(start, inputString.length() - start);
		tokenisedInput.push_back(token);
		start = end + 1;
	} while (end > 0);

	return tokenisedInput;
}


//Processing the user input to run commands
void MerkelMain::inputProcess(std::string input)
{
	std::vector<std::string> tokenisedUserInput = inputTokeniser(input);



	if (tokenisedUserInput[0] == "help")
	{
		helpCommand();
	}

	else if (tokenisedUserInput[0] == "prod" && tokenisedUserInput.size() == 1)
	{
		prodCommand();
	}

	else if (tokenisedUserInput[0] == "min")
	{
		if (tokenisedUserInput.size() == 3) //Checks if the input is valid and will not cause a crash
		{
			minCommand(tokenisedUserInput);
		}
		else
		{
			std::cout << "Vader: Your lack of valid input disturbing me!" << std::endl; //If the input is invalid due to size, will ask a new input
		}
	}

	else if (tokenisedUserInput[0] == "max")
	{
		if (tokenisedUserInput.size() == 3) //Checks if the input is valid and will not cause a crash
		{
			maxCommand(tokenisedUserInput);
		}
		else
		{
			std::cout << "Vader: Your lack of valid input disturbing me!" << std::endl; //If the input is invalid due to size, will ask a new input
		}
	}

	else if (tokenisedUserInput[0] == "avg")
	{
		if (tokenisedUserInput.size() == 4) //Checks if the input is valid and will not cause a crash
		{
			avgCommand(tokenisedUserInput);
		}
		else
		{
			std::cout << "Vader: I am most displeased with your apparent lack of valid input." << std::endl; //If the input is invalid due to size, will ask a new input
		}
	}

	else if (tokenisedUserInput[0] == "predict")
	{
		if (tokenisedUserInput.size() == 4) //Checks if the input is valid and will not cause a crash
		{
			predictCommand(tokenisedUserInput);
		}
		else
		{
			std::cout << "Vader: I see you have constructed a new invalid input." << std::endl; //If the input is invalid due to size, will ask a new input
		}
	}

	else if (tokenisedUserInput[0] == "time" && tokenisedUserInput.size() == 1)
	{
		timeCommand();
	}

	else if (tokenisedUserInput[0] == "step" && tokenisedUserInput.size() == 1)
	{
		stepCommand();
	}

	else if (tokenisedUserInput[0] == "change" && tokenisedUserInput.size() == 4)
	{
		changeCommand(tokenisedUserInput);
	}

	else
	{
		std::cout << "Vader: There is no '" << input << "'. You have failed me again with another invalid input" << std::endl;
	}

}



//Runs the help command, called by writing "help" in advisor bot mode.
//Lists all available commands
void MerkelMain::helpCommand()
{
	std::cout << "Vader: You have only begun to discover your power. Join me and I will complete your training." << std::endl;
}

//Runs the prod command, called by writing "prod" in advisor bot mode
//Lists all available products
void MerkelMain::prodCommand()
{
	int counter = 0; //Counter to count products

	std::cout << "Vader: Now, I will show you the true nature of the Rich Side. Here are all the available products to exchange." << std::endl;

	for (std::string const& p : orderBook.getKnownProducts())
	{
		counter++;
		std::cout << counter << " : " << p << std::endl;
	}

	counter = 0; //resetting the counter to zero again
}

//Runs the min command, called by writing "min" in advisor bot mode
//Finds the minimum bid or ask for product in current time step
void MerkelMain::minCommand(std::vector<std::string> input)
{
	//will use this for dealing with bad inputs
	int productValidity = 0;

	//Checks if the product input is valid (if the Order Book contains the product or not)
	for (std::string const& p : orderBook.getKnownProducts())
	{
		if (p == input[1])
		{
			productValidity++;
		}
	}

	//If the product input is invalid, return with an error message
	if (productValidity == 0)
	{
		std::cout << "Vader: There is no '" << input[1] << "'. You are unwise to search for an invalid product" << std::endl;
		return;
	}

	//If the orderType input is invalid, return with an error message
	if (input[2] != "ask" || input[2] != "bid")
	{
		std::cout << "Vader: There is no '" << input[2] << "'. Search your feelings, you know it to be either 'ask' or 'bid'" << std::endl;
		return;
	}

	std::string productType = input[1];
	OrderBookType saleType = OrderBookEntry::stringToOrderBookType(input[2]);

	std::vector<OrderBookEntry> entries = orderBook.getOrders(saleType, productType, currentTime);
	double minPrice = OrderBook::getLowPrice(entries);

	std::cout << "Vader: The minimum " << input[2] << " for " << productType << " is " << minPrice << " at the moment." << std::endl;
}


//Runs the max command, called by writing "max" in advisor bot mode
//Finds the maximum bid or ask for product in current time step
void MerkelMain::maxCommand(std::vector<std::string> input)
{
	//will use this for dealing with bad inputs
	int productValidity = 0;

	//Checks if the product input is valid (if the Order Book contains the product or not)
	for (std::string const& p : orderBook.getKnownProducts())
	{
		if (p == input[1])
		{
			productValidity++;
		}
	}

	//If the product input is invalid, return with an error message
	if (productValidity == 0)
	{
		std::cout << "Vader: There is no '" << input[1] << "'. You are unwise to search for an invalid product" << std::endl;
		return;
	}

	//If the orderType input is invalid, return with an error message
	if (input[2] != "ask" || input[2] != "bid")
	{
		std::cout << "Vader: There is no '" << input[2] << "'. Search your feelings, you know it to be either 'ask' or 'bid'" << std::endl;
		return;
	}

	std::string productType = input[1];
	OrderBookType saleType = OrderBookEntry::stringToOrderBookType(input[2]);

	std::vector<OrderBookEntry> entries = orderBook.getOrders(saleType, productType, currentTime);
	double maxPrice = OrderBook::getHighPrice(entries);

	std::cout << "Vader: The maximum " << input[2] << " for " << productType << " is " << maxPrice << " at the moment." << std::endl;
}

//Runs the max command, called by writing "max" in advisor bot mode
//Computes average ask or bid for the sent product over the sent number of time steps.
void MerkelMain::avgCommand(std::vector<std::string> input)
{
	//will use these for dealing with bad inputs
	int productValidity = 0;
	int pastTimeNumber = orderBook.checkPastTimes(currentTime);


	int digitizedInput;
	double average;

	//converting string to a number
	try
	{
		digitizedInput = std::stoi(input[3]);
	}
	catch (const std::exception& e)
	{
		std::cout << "Vader: From my point of view '." << input[3] << "' is not a number" << std::endl;
		return;
	}

	//checks if the input includes '.' or ',' using ASCII codes. If it includes it (and already past the string to integer check) then it means it's a float but not an integer.
	for (char c : input[3])
	{
		if (c == 44 || c == 46)
		{
			std::cout << "Vader: No... '" << input[3] << "' is not an integer" << std::endl;
			return;
		}
	}

	//Checks if the product input is valid (if the Order Book contains the product or not)
	for (std::string const& p : orderBook.getKnownProducts())
	{
		if (p == input[1])
		{
			productValidity++;
		}
	}

	//If the product input is invalid, return with an error message
	if (productValidity == 0)
	{
		std::cout << "Vader: There is no '" << input[1] << "'. You are unwise to search for an invalid product" << std::endl;
		return;
	}

	//If the orderType input is invalid, return with an error message
	if (input[2] != "ask" && input[2] != "bid")
	{
		std::cout << input[2].size() << std::endl;
		std::cout << "Vader: There is no '" << input[2] << "'. Search your feelings, you know it to be either 'ask' or 'bid'" << std::endl;
		return;
	}

	//If there isn't much past steps yet, changes digitized input so that the calculation will be from the beginning of the order
	if (digitizedInput >= pastTimeNumber)
	{
		std::cout << "Vader: The validity of your input is strong. But there is no '" << input[3] << "' past steps yet." << std::endl;
		std::cout << "       Now, I will show you the average price since the first time step" << std::endl;
		digitizedInput = pastTimeNumber - 1;
	}

	std::vector<std::string> timestampsVector = orderBook.getVectorOfPreviousSteps(digitizedInput, currentTime);

	average = orderBook.calculateAveragePrice(timestampsVector, input[1], input[2]);

	std::cout << average << std::endl;

}



//Runs the predict command, called by writing "predict" in advisor bot mode
//Computes the predicition for the max/min ask/bid price for a given product at the next timestep
void MerkelMain::predictCommand(std::vector<std::string> input)
{
	currentTime = "2020/03/17 17:02:00.124758";
	double EMA; //Exponential Moving Average
	double avgPriceDifference; //The average difference from the average prices

	//will use this for dealing with bad inputs
	int productValidity = 0;

	//Checks if the product input is valid (if the Order Book contains the product or not)
	for (std::string const& p : orderBook.getKnownProducts())
	{
		if (p == input[2])
		{
			productValidity++;
		}
	}

	//If the product input is invalid, return with an error message
	if (productValidity == 0)
	{
		std::cout << "Vader: There is no '" << input[2] << "'. You are unwise to search for an invalid product" << std::endl;
		return;
	}

	//If the orderType input is invalid, return with an error message
	if (input[3] != "ask" && input[3] != "bid")
	{
		std::cout << input[2].size() << std::endl;
		std::cout << "Vader: There is no '" << input[3] << "'. Search your feelings, you know it to be either 'ask' or 'bid'" << std::endl;
		return;
	}

	//Calculating the Exponential Moving Average
	EMA = orderBook.calculateMovingAverage(currentTime, input[2], input[3]);

	std::cout << "Vader: Your destiny lies with the profit. " << std::endl;
	std::cout << "   - Calculated Exponential Moving Average for the next step is: " << EMA << std::endl;

	//Creating a vector of timestamps from the beginning until the current time
	std::vector<std::string> timestampsVector = orderBook.getVectorOfAllPreviousAndCurrentSteps(currentTime);

	//Calculating the average difference until the current time
	avgPriceDifference = orderBook.predictPriceDifference(timestampsVector, input[2], input[3], input[1]);
	std::cout << "   - Predicted " << input[1] << " price for the next " << input[3] << " of " << input[2] << " is: " << (EMA + avgPriceDifference) << std::endl;
}


//Runs the time command, called by writing "time" in advisor bot mode
//Prompts the current timestep
void MerkelMain::timeCommand()
{
	std::cout << "Vader: The current time step is : '" << currentTime << "'. When I left you, I was but the poorer. Now I am the master." << std::endl;
}


//Runs the step command, called by writing "step" in advisor bot mode
//Moves to the next time step using gotoNextTimeFrame()
void MerkelMain::stepCommand()
{
	std::cout << "Vader: It is your destiny. Join me, and together we can move to the next time step : " << currentTime << std::endl;
	gotoNextTimeframe();
}

//Runs the change command, called by writing "step" in advisor bot mode.
//Prints how much a product price is changed since a given input or in maximum time steps
void MerkelMain::changeCommand(std::vector<std::string> input)
{
	//will use these for dealing with bad inputs
	int productValidity = 0;
	int pastTimeNumber = orderBook.checkPastTimes(currentTime);
	std::string reference_step;


	int digitizedInput;
	double change;

	//converting string to a number
	try
	{
		digitizedInput = std::stoi(input[3]);
	}
	catch (const std::exception& e)
	{
		std::cout << "Vader: From my point of view '." << input[3] << "' is not a number" << std::endl;
		return;
	}

	//checks if the input includes '.' or ',' using ASCII codes. If it includes it (and already past the string to integer check) then it means it's a float but not an integer.
	for (char c : input[3])
	{
		if (c == 44 || c == 46)
		{
			std::cout << "Vader: No... '" << input[3] << "' is not an integer" << std::endl;
			return;
		}
	}

	//Checks if the product input is valid (if the Order Book contains the product or not)
	for (std::string const& p : orderBook.getKnownProducts())
	{
		if (p == input[1])
		{
			productValidity++;
		}
	}

	//If the product input is invalid, return with an error message
	if (productValidity == 0)
	{
		std::cout << "Vader: There is no '" << input[1] << "'. You are unwise to search for an invalid product" << std::endl;
		return;
	}

	//If the orderType input is invalid, return with an error message
	if (input[2] != "ask" && input[2] != "bid")
	{
		std::cout << input[2].size() << std::endl;
		std::cout << "Vader: There is no '" << input[2] << "'. Search your feelings, you know it to be either 'ask' or 'bid'" << std::endl;
		return;
	}

	//If there isn't much past steps yet, changes digitized input so that the calculation will be from the beginning of the order
	if (digitizedInput >= pastTimeNumber)
	{
		std::cout << "Vader: The validity of your input is strong. But there is no '" << input[3] << "' past steps yet." << std::endl;
		std::cout << "       Now, I will show you the percentage change of "<< input[2] <<"s since the first time step" << std::endl;
		digitizedInput = pastTimeNumber - 1;
	}

	else if (digitizedInput <= 0)
	{
		std::cout << "Vader: No... '" << input[3] << "' is not a valid step input. Give me something bigger than zero" << std::endl;
		return;
	}

	std::string referenceTimestamp = orderBook.getReferencedPreviousTimestamp(digitizedInput, currentTime); //Gets the reference timestamp for 'x' timesteps before
	change = orderBook.calculateChangePrice(digitizedInput, currentTime, input[1], input[2]); //Calculates the change


	if (change == 0)
	{
		std::cout << "Vader: The price is not changed or have only changed insignificantly" << std::endl;
		return;
	}

	else if (change == -1000000)
	{
		std::cout << "Vader: There have been no " << input[2] << " for" << input[1] << "until " << referenceTimestamp;
		return;
	}

	std::cout << "The " << input[2] << " price for " << input[1] << " is changed by " << change << "%." << std::endl;
}
