#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class MerkelMain
{
public:
    MerkelMain();
    /** Call this to start the sim */
    void init();

private:
    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();
    int getUserOption();
    void processUserOption(int userOption);

	std::string currentTime;

	OrderBook orderBook{ "20200601.csv" };
	Wallet wallet;


	//FUNCTIONS I HAVE WRITTEN

	//Function to initiate and manage the Advisor Bot
    void manageAdvisorBot(); 

	//Processing function for user Inputs
	void inputProcess(std::string input);

	//Function to tokenise user input in order to process the input
	std::vector<std::string> inputTokeniser(std::string inputString);

	//Lists all available commands
	void helpCommand();

	//Prints the function of a command and how to use it
	void helpcmdCommand(std::vector<std::string> input);

	//Lists all available products
	void prodCommand();

	//Finds the minimum bid or ask for product in current time step
	void minCommand(std::vector<std::string> input);

	//Finds the maximum bid or ask for product in current time step
	void maxCommand(std::vector<std::string> input);

	//Computes average ask or bid for the sent product over the sent number of time steps.
	void avgCommand(std::vector<std::string> input);

	//Computes the predicition for the max/min ask/bid price for a given product at the next timestep
	void predictCommand(std::vector<std::string> input);

	//Prompts the current timestep
	void timeCommand();

	//Moves to the next time step, makes the exchanges and prompts it
	void stepCommand();

	//Calculates how much the bid or ask price changed from since a given step input (e.g 5 steps earlier) until the current time
	void changeCommand(std::vector<std::string> input);


};
