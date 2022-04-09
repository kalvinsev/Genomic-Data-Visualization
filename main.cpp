// Name: Kalvin Sevillano
// SSID: 1639328
// Assignment #: 4
// Submission Date: 05/15/20
// Description of program: The program demonstrated below creates a DNA sequence of length(k) entered by the user of which we then 
//use a pixel indexing alogrithm to develop an image based off the frequencies of such DNA sequences.

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility> 
#include <math.h>
#include <bitset>
#include <cstdlib>

typedef unsigned char uchar;
struct Color{ 
    
    uchar rgb[3]; 
    
};

void createCoord(char c[], int k, std::string &xbinary, std:: string &ybinary);

bool legal(std::string str); // tests if k value recieved is a valid k value (integer value)
void create_map(std::string txtFile, int k);
void create_kword(std::string k_word, std::ifstream &frequency_file, int k, std::string &txtFile);
void create_mapFile(std::map<std::string, int> frequency_table, std::string txtFile, int k);
void create_mapPair(std::string k_word, int k); // creating map pair from k-length string
void writeToColorPGMImages(uchar** images, int img_size, Color bgk, std::string &txtFile, int k);


std::map<std::string, int> frequency_table; //declaring an empty map
Color bgk;

int main(){
    
    std::string txtFile, str;   //txt file stores txt file selected from user, str stores our unknown k value
    int k;
    std::cout << "Enter in a sequence filename(e.g. sars.txt, etc) or choose from the list below:\n" <<
                 "0. sars.txt\n" <<
                 "1. bat_cov.txt\n" <<
                 "2. pigcov.txt\n" <<
                 "3. mers.txt\n" << 
                 "4. hiv.txt\n" <<
                 "5. sars_cov_2.txt\n"<<
                 "Type in the file name: ";
    std::cin >> txtFile;
    std::cout << "Enter a value (k) for the length of words to use for the k-mer visualization: ";
    std::cin >> str;
    legal(str);
    k = stoi(str); //string to int(k)
    create_map(txtFile, k); 
    return 0;

}

void create_map(std::string txtFile, int k){
    std::string  k_word = "";       //empty string used to read over words in file of length k
    std::ifstream frequency_file; 
    frequency_file.open(txtFile);   //conecting to user provided file
    create_kword(k_word, frequency_file, k, txtFile);
    frequency_file.close();
    
    
}
void create_mapFile(std::map<std::string, int> frequency_table, std::string txtFile, int k){
    std::ofstream writeMap;
    txtFile += "grid.txt";
    // std::cout << txtFile << '\n';
    writeMap.open(txtFile);
    int newline(0), HL = pow(2,k); // HL = horziontal length (2^k)
    for (const auto &p : frequency_table) {
            if(newline % HL == 0)
                writeMap << '\n';
            writeMap << p.first << " : " << p.second << ' '; //<< '\n';
            newline++;
    }
    frequency_table.clear(); //==================Clears map==========================
    writeMap.close();
    std::cout << '\n' << "Frequency grid written to file: " << txtFile;
}



void create_kword(std::string k_word, std::ifstream &frequency_file, int k, std::string &txtFile){
    int arrSize = pow(2,k);
    int** tempArr = new int*[arrSize];
    uchar** images = new uchar*[arrSize];
    int xc, yc;
    for (int i(0); i < arrSize; i++){
        tempArr[i] = new int[arrSize];
        images[i] = new uchar[arrSize];
    }
        
    int cursor(0);
    while(frequency_file.seekg(cursor, std::ios::beg)){// loop increments over one byte in file for each char in file and begins reading from the next char
        
        char c[k];
        for (int i(0); i < k; i++){
            frequency_file >> c[i]; // get each char 
            k_word += c[i]; //concatenate each char to string k_word
        }
        std::string xbinary = "";  //empty binary strings
        std::string ybinary = "";
        createCoord(c, k, xbinary, ybinary); //function call to fill binary strings.
        std::bitset<8> x(xbinary); 
        std::bitset<8> y(ybinary);
        xc = x.to_ulong(); 
        yc = y.to_ulong();
        create_mapPair(k_word, k);  // line 
        frequency_table[k_word]++;
        std::map<std::string, int>::iterator it = frequency_table.find(k_word);
        if (it !=frequency_table.end()){
            tempArr[xc][yc]++;
        }
            
        k_word.clear();    
        cursor++;
    }
    // Iterate through the map
    auto it = frequency_table.begin();
	while (it != frequency_table.end()){
		// Check if value of this entry matches with given value
		if (it->second == 1)
			it = frequency_table.erase(it);     // Erase the current element
		
		else
			it++;   // Shift over to next key value in map
	}

    int newline(0), HL = pow(2,k); // HL = horziontal length (2^k)
    if (k <= 3){ 
        for (const auto &p : frequency_table) {
            if(newline % HL == 0 /*&& newline!=0*/)
                std::cout << '\n';
            std::cout << p.first << " : " << p.second << ' '; //<< '\n';
            newline++;
        }
    }
       

    
    //display 2D Array
  
    for (int x(0); x < arrSize; x++)
        for (int y(0); y < arrSize; y++){
        images[x][y] = static_cast<uchar>(tempArr[x][y]);
    }
        
    create_mapFile(frequency_table, txtFile, k);
    char yn;
    std::cout << "\nDo you want to save the image to file? Y/N ";
    std::cin >> yn;
    if (yn == 'Y'|| yn == 'y'){
        writeToColorPGMImages(images, arrSize, bgk, txtFile, k);
        
    }
    else{
        std::cout << "See ya nextime." << '\n';
        delete[] images;
        delete[] tempArr;
        exit(1);
    }
    
    std::cout << "\nDo you want to see a different visualization? Y/N ";
    std::cin >> yn;
    if (yn == 'Y'|| yn == 'y'){
        frequency_table.clear();
        delete[] images;
        delete[] tempArr;
        main();
    }
    else{
        std::cout << "See ya nextime." << '\n';
        exit(1);
    }

    
}

void create_mapPair(std::string k_word, int k){
    int i(0);
    frequency_table.insert(std::make_pair(k_word, i));
}

bool legal(std::string str){  // checks if entry is legal
    for (int i = 0; i < str.length(); i++){
         if(str == "0"){
            std::cout << "Invalid input\n";
            main();
        }
        else if (str == "1")
            return true;
        else if (isdigit(str[i]) == false){
            std::cout << "Invalid input\n";
            main();
        }
    }
       
    return true;
}

void createCoord(char c[], int k, std::string &xbinary, std:: string &ybinary){
    for (int x(0); x < k; x++){
        if (c[x]=='A' || c[x] == 'C')
            xbinary += "0";
        else if (c[x] == 'G' || c[x] == 'T')  //creates x coord from given algo
            xbinary += "1";
    }
    for (int y(0); y < k; y++){
        if (c[y]=='A' || c[y] == 'G')
            ybinary += "0";
        else if (c[y] == 'C' || c[y] == 'T') //creates y coord from given algo
            ybinary += "1";
    }
}
void writeToColorPGMImages(uchar** images, int img_size, Color bgk, std::string &txtFile, int k){

    std::ofstream write_to_PPM;
    write_to_PPM.open(txtFile + "_k_.ppm");
    int temp_bgk[3];
    double** temp_images = new double*[img_size];
    write_to_PPM << "P3" << "\n" << pow(2,k) << ' ' << pow(2,k) << "\n" << "255" << "\n";
    for (int i(0); i < img_size; i++){
        temp_images[i] = new double[img_size];
    }

    for (int x(0); x < img_size; x++)
        for (int y(0); y < img_size; y++){
            temp_images[x][y] = static_cast<double>(images[x][y]);
        }

    double max = temp_images[0][0];        
    for (int x(0); x < img_size; x++)
        for (int y(0); y < img_size; y++)
            if (temp_images[x][y] > max)
                max = temp_images[x][y];

    for (int x(0); x < img_size; x++)
        for (int y(0); y < img_size; y++){
            double cNumber = (1-((temp_images[x][y])/max)*255);
            for(int i(0); i < 3; i++){
                double grayscale = cNumber;
                bgk.rgb[i] = static_cast<int>(cNumber);                     ///writing to file
                write_to_PPM << static_cast<int>(bgk.rgb[i]) << ' ';
                if (x % img_size == 0)
                    write_to_PPM << '\n';
            }

        }
    
    write_to_PPM.close();
    std::cout << "File saved under " << txtFile << "_k_.ppm";


}

