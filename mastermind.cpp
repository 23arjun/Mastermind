

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <chrono>

int twod_to_oned(int row, int col, int rowlen);

void set_random_seed();
int randn(int n);


struct mm_code_maker{


    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;
    }


    void generate_sequence(){
        for(int i = 0; i < length; i++){
            sequence.push_back(randn(num));
        }
    }

    void give_feedback(const std::vector<int>& attempt, int& black_hits, int& white_hits){

        std::vector<int> a2;
        std::vector<int> s2;
        a2.clear();
        s2.clear();
        black_hits = 0;
        white_hits = 0;

        for(int i = 0; i < attempt.size(); i++){
            if(attempt[i] == sequence[i]){
                black_hits++;
            }

            else{
                a2.push_back(attempt[i]);
                s2.push_back(sequence[i]);
            }
        }
        int i = 0;
        int j = 0;
        bool matchfound = false;

        while(i<a2.size()){
            j = 0;
            while(j < s2.size() && matchfound == false){
                if(a2[i] == s2[j]){
                    matchfound = true;
                }
                else{
                    j++;
                }
            }

            if(matchfound){
                white_hits++;
                a2.erase(a2.begin() + i);
                s2.erase(s2.begin() + j);
                matchfound = false;

            }
            else{
                i++;

            }
        }
    }

    std::vector<int> sequence;

    int length;
    int num;

};

struct mm_solver{

    std::vector<int> tmp;

    void init(int i_length, int i_num){
        length = i_length;
        num = i_num;


        if(std::pow(num, length) >= std::pow(8,7)){
            for(int i = 0; i < num; i ++){
                for(int j = 0; j < length; j++){
                    tmp.push_back(i);
                }
                v.push_back(tmp);
                tmp.clear();
            }


            initial_attempts = true;
            next_col = 0;
            previous_bh = 0;
            previous_wh = 0;
            numcount = 0;
            numattempts = 0;
        }
    }

    void total_set(int num, int length){


        std::vector<int> permutations;

        for(int z = 0; z < (std::pow(num, length))*length; z++){
            permutations.push_back(0);
        }


        int possibilities = length* std::pow(num, length);
        for(int i = 0; i < length; i++){
            int j = 0;
            while(j < possibilities/length){
                int fill = std::pow(num, i);
                for(int l = 0; l < num; l++){
                    for (int k = 0; k < fill ; k++){
                        permutations[twod_to_oned(j, (length-1)-i, length)] = l;
                        j++;
                    }
                }
            }
        }

        for(int i = 0; i < permutations.size()/length; i++){
            for(int j = 0; j < length; j++){
                tmp.push_back(permutations[twod_to_oned(i,j ,length)]);
            }
            v.push_back(tmp);
            tmp.clear();
        }
    }

    void create_attempt(std::vector<int>& attempt){

        if(std::pow(num, length) <= std::pow(8,7)){
            create_attemptKnuth(attempt);
        }
        else{
            create_attemptLongCodes(attempt);
        }



    }


    void create_attemptKnuth(std::vector<int>& attempt){



        if(first_attempt){

            if(num == 1){
                for(int i = 0; i < length; i++){
                    attempt.push_back(0);
                }
            }


            else{
                for(int i = 0; i < length/2; i++){
                    attempt.push_back(0);
                }

                for(int j = 0; j<length/2; j++){
                    attempt.push_back(1);
                }

                if(length%2 != 0){
                    if(num>2){
                        attempt.push_back(2);
                    }
                    else{
                        attempt.push_back(1);
                    }
                }
            }
            first_attempt = false;
        }

        else{

            attempt.clear();
            if(v.size() != 1){
                int x = randn(v.size()-1);
                for(int j = 0; j < length; j++){
                    int tmp = v[x][j];
                    attempt.push_back(tmp);
                }
            }
            else{
                for(int j = 0; j < length; j++){
                    int tmp = v[0][j];
                    attempt.push_back(tmp);
                }
            }
        }
    }

    void create_attemptLongCodes(std::vector<int>& attempt){

        initial_attempts = true;


        if(numattempts < num && initial_attempts){

            attempt.clear();

            for(int i = 0; i< length; i++){
                int tmp = v[numattempts][i];
                attempt.push_back(tmp);
            }

            numattempts++;
            initial_attempts = false;
        }


        else if (numattempts >= num && initial_attempts){
            attempt.clear();

            attempt = attemptbuffer;
            numattempts++;
            initial_attempts = false;
        }
    }

    void learn(std::vector<int>& attempt, int black_hits, int white_hits){

        if(std::pow(num, length) <= std::pow(8,7)){
            learnKnuth(attempt, black_hits, white_hits);
        }
        else{
            learnLongCodes(attempt, black_hits, white_hits);
        }
    }


    void learnKnuth(std::vector<int>& attempt, int black_hits, int white_hits){

        std::vector<int> row;
        std::vector<int> possibility;
        std::vector<int> tmp;
        int bh = 0;
        int wh = 0;

        if(first_attempt2){
            total_set(num, length);


            first_attempt2 = false;
        }

        for(int i = 0; i < v.size(); i++){
            for(int j = 0; j < v[i].size(); j++){
                possibility.push_back(v[i][j]);
            }

            givefeedback(possibility, attempt, bh, wh);


            if(wh == white_hits && bh == black_hits){
                v_red.push_back(possibility);
            }
            possibility.clear();
        }


        v.clear();

        for(int i = 0; i < v_red.size(); i++){
            for(int j = 0; j <v_red[i].size(); j++){
                int temp = v_red[i][j];
                row.push_back(temp);
            }

            v.push_back(row);
            row.clear();

        }
        v_red.clear();

    }

    void learnLongCodes(std::vector<int>& attempt, int black_hits, int white_hits){


        //initial_attempts2 = true;
        if(numattempts <= num){

            for(int i = 0; i < black_hits; i++){
                numbers.push_back(attempt[i]);
            }
            if(black_hits > mostfreqbh){
                firstfromnum = attempt[0];
                mostfreqbh = black_hits;
            }
        }





        if(numattempts == num){

            attemptbuffer.clear();

            unit = numbers[0];

            for(int i = 0; i < length; i++){
                attemptbuffer.push_back(unit);
            }




            for(int i = 0; i < numbers.size(); i++){
                if(numbers[i] == unit){
                    numbers.erase(numbers.begin() + i );
                    //  i--;
                    next_col = 0;
                }
            }
        }



        if(numattempts == num +1){
            attemptbuffer[0] = numbers[0];
        }

        if(numattempts > num+1 && black_hits < length){

            if( black_hits > previous_bh && white_hits == 0){
                //std::cout <<"hibh>" << std::endl;

                if(numcount > 0){
                    numbers.erase(numbers.begin() + numcount - 1);

                }

                else {

                    numbers.erase(numbers.begin() + numcount);

                }
                next_col++;
                numcount = 0;
                attemptbuffer[next_col] = numbers[numcount];

                numcount++;
                if(numcount > numbers.size()){
                    numcount = 0;
                }
            }

            if(white_hits == 2 && next_col < attempt.size() -1){
                //std::cout <<"hiwh2" << std::endl;
                int tmp = attemptbuffer[next_col];
                attemptbuffer[next_col] = attempt[next_col+1];
                attemptbuffer[next_col + 1] = tmp;

                next_col++;

            }

                if(black_hits == previous_bh && white_hits != 2){
                                        //std::cout <<"hibh=" << std::endl;

                                        attemptbuffer[next_col] = numbers[numcount];
                                        numcount++;
                                    if(numcount > numbers.size()){
                                            numcount = 0;
                            }
                                    }

                            if(previous_bh < black_hits && white_hits == 1){

                                        attemptbuffer[next_col] = numbers[numcount];
                                        numcount++;


                                        if(numcount > numbers.size()){
                                            numcount = 0;
                                        }
                                    }
                                    }

                                    previous_bh = black_hits;
                                    previous_wh = white_hits;

                                    }


    void givefeedback(std::vector<int>& possibility, const std::vector<int>& attempt, int& bh, int& wh){

        std::vector<int> a2;
        std::vector<int> s2;
        a2.clear();
        s2.clear();
        bh = 0;
        wh = 0;

        for(int i = 0; i < attempt.size(); i++){
            if(possibility[i] == attempt[i]){
                bh++;
            }

            else{
                a2.push_back(possibility[i]);
                s2.push_back(attempt[i]);
            }
        }

        int i = 0;
        int j = 0;
        bool matchfound = false;

        while(i<a2.size()){
            j = 0;
            while(j < s2.size() && matchfound == false){
                if(a2[i] == s2[j]){
                    matchfound = true;
                }
                else{
                    j++;
                }
            }

            if(matchfound){
                wh++;
                a2.erase(a2.begin() + i);
                s2.erase(s2.begin() + j);
                matchfound = false;

            }
            else{
                i++;
            }
        }
    }
    //common variable
    std::vector<std::vector<int> > v;

    //knuth variables
    int length;
    int num;
    bool first_attempt = true;
    bool first_attempt2 = true;

    std::vector<std::vector<int> > v_red;

    //long variables
    int unit;
    std::vector<int> numbers;
    std::vector<int> previous_attempt;
    bool initial_attempts;
    int numattempts;
    int numattempts2;
    bool firstfromhits;
    int previous_bh;
    int previous_wh;
    int next_col;
    std::vector<int> attemptbuffer;
    int numcount;
    int mostfreqbh;
    int firstfromnum;

};

int main(){

    set_random_seed();

    int length, num;
    std::cout << "enter length of sequence and number of possible values:" << std::endl;
    std::cin >> length >> num;

    mm_solver solver;
    solver.init(length, num);

    mm_code_maker maker;
    maker.init(length, num);
    maker.generate_sequence();

    int black_hits=0, white_hits=0;
    int attempts_limit = 5000;
    int attempts = 0;
    while((black_hits < length) && (attempts < attempts_limit)){
        std::vector<int> attempt;
        solver.create_attempt(attempt);
        maker.give_feedback(attempt, black_hits, white_hits);
        std::cout << "attempt: " << std::endl;
        for(int i = 0; i < attempt.size(); i++){
            std::cout << attempt[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;
        solver.learn(attempt, black_hits, white_hits);
        attempts++;
    }

    if(black_hits == length){
        std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
    }
    else{
        std::cout << "after " << attempts << " attempts still no solution" << std::endl;
    }
    std::cout << "the sequence generated by the code maker was:" << std::endl;
    for(int i = 0; i < maker.sequence.size(); i++){
        std::cout << maker.sequence[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}



/// not a great implementation for set_random_seed and for randn;
/// if you are trying to get better results you may want to change
/// the implementation using C++11 features, see for instance
/// https://isocpp.org/files/papers/n3551.pdf
/// but don't change the interface/declaration of the functions

void set_random_seed(){
    std::srand(std::time(0));
}

int randn(int n){
    return std::rand()%n;
}


int twod_to_oned(int row, int col, int rowlen){
    return row*rowlen+col;
}
