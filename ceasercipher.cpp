//c++ ceaser cipher
#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <algorithm>
#include <memory>

class Letter{
	private:
		const int lowerCaseBegin = 97;
		const int lowerCaseEnd   = 122;
		const int upperCaseBegin = 65;
		const int upperCaseEnd   = 90;
	public:
		bool  operator==(const char rhs) const{
			bool isLetter = false;
			if ((rhs >= lowerCaseBegin && rhs <= lowerCaseEnd) || (rhs >= upperCaseBegin && rhs <= upperCaseEnd))
				isLetter = true;
			return isLetter;
		}
		int   getLowerBegin() const{return lowerCaseBegin;}
		int   getLowerEnd() const{return lowerCaseEnd;}
		int   getUpperBegin() const{return upperCaseBegin;}
		int   getUpperEnd() const{return upperCaseEnd;}
		bool  isLower(const char ch) const{
			bool lower = false;
			if (ch >= getLowerBegin() && ch <= getLowerEnd())
				lower = true;
			return lower;
		}
		bool  isUpper(const char ch) const{
			bool upper = false;
			if (ch >= getUpperBegin() && ch <= getUpperEnd())
				upper = true;
			return upper;
		}
};

class Shift{
	private:
		Letter verifyLetter;
		std::string  phrase{};
		int          k{};
		void makeShift(){
			auto func = [this](char& ch){
				if (this->verifyLetter == ch){
					int k = this->k;
					if (this->verifyLetter.isLower(ch)){
						if ((ch + k) <= verifyLetter.getLowerEnd()){
							ch += k;
						}
						else{
							k = (ch + k) - verifyLetter.getLowerEnd();
							ch = verifyLetter.getLowerBegin() + k;
						}
					}
					else{
						if ((ch + k) <= verifyLetter.getUpperEnd()){
							ch += k;
						}
						else{
							k = (ch + k) - verifyLetter.getUpperEnd();
							ch = verifyLetter.getUpperBegin() + k;
						}
					}
				}
			};

			std::for_each(phrase.begin(), phrase.end(), func);
		}

		void undoShift(){
			auto func = [this](char& ch){
				if (this->verifyLetter == ch){
					int k = this->k;
					if (this->verifyLetter.isLower(ch)){
						if ((ch - k) >= verifyLetter.getLowerBegin()){
							ch -= k;
						}
						else{
							k = verifyLetter.getLowerBegin() - (ch - k);
							ch = verifyLetter.getLowerEnd() - k;
						}
					}
					else{
						if ((ch - k) >= verifyLetter.getUpperBegin()){
							ch -= k;
						}
						else{
							k = verifyLetter.getUpperBegin() - (ch - k);
							ch = verifyLetter.getUpperEnd() - k;
						}
					}
				}
			};

			std::for_each(phrase.begin(), phrase.end(), func);
		}
	public:
		Shift(std::string phrase, int k, char action){
			this->phrase = phrase;
			if (k >= 0 && k <= 26)
				this->k = k;
			else
				throw std::invalid_argument{"K can't be negative or greater than 26\n"};
			if (action == 'e')
				makeShift();
			else
				undoShift();
		}
		Shift(std::ifstream& strm, int k, char action){
			if (k >= 0 && k <= 26)
				this->k = k;
			else
				throw std::invalid_argument{"K can't be negative or greater than 26\n"};
			if (strm.is_open()){
				while (!strm.eof()){
					std::string temp{};
					std::getline(strm, temp);
					phrase += temp + '\n';
				}
				if (action == 'e')
					makeShift();
				else
					undoShift();
			}
			else{
				throw std::runtime_error{"Bad Stream\n"};
			}
		}
		std::string getPhrase(){
			return phrase;
		}

};

int main()
{
	std::string  phrase;
	std::ifstream messageStrm;
	int k{};
	int option{};

	std::unique_ptr<Shift> cipher;

	std::cout << "This program will encrypt a message using the ceaser cipher.\n"
			  << "To use the program input a k value that will be used for your encryption and select the type\n"
			  << "of input you would like to use for your entering your message.\n\n";
	std::cout << "If you would like to read the message from a txt file you have enter 1.\n"
			  << "If you would like to type the message in press 2.\n"
			  << "If you would like to decode a message press 3.\n"
			  << "-";
    std::cin >> option;
    while (option < 1 || option > 3){
    	std::cout << "Wrong input try again: ";
    	std::cin >> option;
    }

    std::cout << "Enter your k value: ";
    std::cin >> k;

    if (option == 1){
    	std::string filename;
    	std::cout << "Please input the filename to read from: ";
    	std::cin >> filename;
    	messageStrm.open(filename.c_str(), std::ios::in);
    	cipher = std::unique_ptr<Shift>(new Shift{messageStrm, k, 'e'});
    	messageStrm.close();
    }
    else if (option == 2){
    	std::cout << "Enter your message(press tab when you are done): ";
 		std::getline(std::cin, phrase, '\t');
 		cipher = std::unique_ptr<Shift>(new Shift{phrase, k, 'e'});
    }
    else{
    	std::cout << "Enter 1 to read cipher from a file or 2 to type it in: ";
    	std::cin >> option;
    	while (option < 1 && option > 2){
    		std::cout << "Wrong input try again: ";
    		std::cin >> option;
    	}
    	if (option == 1){
    		std::string filename;
    		std::cout << "Please input the filename to read from: ";
    		std::cin >> filename;
    		messageStrm.open(filename.c_str(), std::ios::in);
    		cipher = std::unique_ptr<Shift>(new Shift{messageStrm, k, 'd'});
    		messageStrm.close();
    	}
    	else{
    		std::cout << "Enter your message(press tab when you are done): ";
 			std::getline(std::cin, phrase, '\t');
 			cipher = std::unique_ptr<Shift>(new Shift{phrase, k, 'd'});
    	}
    }
    std::cout << "Your encoded message is " << cipher->getPhrase() << std::endl << std::endl;
    std::cout << "If you would like to write decoded/encoded phrase to a file press 1 or anything else to quit: ";
    std::cin >> option;
    if (option == 1){
    	std::string filename;
    	std::cout << "Enter filename: ";
    	std::cin >> filename;
    	std::fstream writeStrm(filename.c_str(), std::ios::out);
    	if (!writeStrm.is_open())
    		throw std::runtime_error{"File in use could not open.\n"};
    	writeStrm << cipher->getPhrase();
    	std::cout << "File created.\n\n";
    	writeStrm.close();
    }

	return 0;
}




