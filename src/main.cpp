#include "e24.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

void printHelp();
void printResult(float vcc, float v_2, float r1, float r2);
void printResult(float vcc, float v_2, std::vector<float> r1, std::vector<float> r2);
float getV2(float vcc, float r1, float r2);
float getR1(float vcc, float v_2, float r2);
float getR2(float vcc, float v_2, float r1);
std::vector<std::pair<unsigned int, unsigned int>> getR1R2(float vcc, float v_2, float i=0.0);
float findE24(float e);

int main(int argc, char *argv[]) {
    bool simpleMode = true;
    
    if(argc < 5 || argc > 6) {
        printHelp();

        return 1;
    }

    float vcc = atof(argv[1]);
    float v_2 = atof(argv[2]);
    float r1 = atof(argv[3]);
    float r2 = atof(argv[4]);
    float i = atof(argv[5]);
    std::vector<float> r1a;
    std::vector<float> r2a;

    if(vcc == 0.0f || r1 == 0.0f) {
        printHelp();
    }

    if(v_2 == -1) {
        v_2 = getV2(vcc, r1, r2);
    } else if(r1 != -1 && r2 == -1) {
        r2 = getR2(vcc, v_2, r1);
    } else if(r1 == -1 && r2 != -1) {
        r1 = getR1(vcc, v_2, r2);
    } else if(r1 == -1 && r2 == -1) {
        std::vector<std::pair<unsigned int, unsigned int>> resistors = getR1R2(vcc, v_2, i);
        for(std::pair<float, float> p : resistors) {
            r1a.emplace_back(p.first);
            r2a.emplace_back(p.second);
        }
        simpleMode = false;
    }

    if(simpleMode) {
        printResult(vcc, v_2, r1, r2);
    } else {
        printResult(vcc, v_2, r1a, r2a);
    }

    return 0;
}

void printHelp() {
    std::cout << "Usage: voltdiv vcc r1\n";
}

void printResult(float vcc, float v_2, float r1, float r2) {
    std::cout << "  |------- " << vcc << " V \n";
    std::cout << "  |\n";
    std::cout << " |\u00AF|\n";
    std::cout << " | |       " << r1 << " \u03A9\n";
    std::cout << " |_|\n";
    std::cout << "  |\n";
    std::cout << "  |------- " << v_2 << " V\n";
    std::cout << "  |\n";
    std::cout << " |\u00AF|\n";
    std::cout << " | |       " << r2 << " \u03A9\n";
    std::cout << " |_|\n";
    std::cout << "  |\n";
    std::cout << "  |\n";
}

void printResult(float vcc, float v_2, std::vector<float> r1, std::vector<float> r2) {
    clear();
    std::cout << "Supply voltage: " << vcc << "\n";
    std::cout << "   R1 [\u03A9]   |" ;
    std::cout << "   R2 [\u03A9]   |" ;
    std::cout << "  V_2 [V] |" ;
    std::cout << "   I [A]\n";
    std::cout << std::setfill('-') << std::setw(48) << "" << "\n";

    char r1s[9];
    char r2s[9];
    
    for(int i=0; i < r1.size(); ++i) {
        if(r1.at(i) > 1000000) {
            sprintf(r1s, "%.1f M", r1.at(i)/1000000);
        } else if(r1.at(i) > 1000) {
            sprintf(r1s, "%.1f k", r1.at(i)/1000);
        } else {
            sprintf(r1s, "%.1f", r1.at(i));
        }

        if(r2.at(i) > 1000000) {
            sprintf(r2s, "%6.1f M", r2.at(i)/1000000);
        } else if(r2.at(i) > 1000) {
            sprintf(r2s, "%6.1f k", r2.at(i)/1000);
        } else {
            sprintf(r2s, "%8.1f", r2.at(i));
        }
        
        printf("            |            |          | ");
        gotoxy(3, 4+i);
        printf("%s", r1s);
        gotoxy(15, 4+i);
        printf("%s", r2s);
        gotoxy(30, 4+i);
        printf("%.1f", getV2(vcc, r1.at(i), r2.at(i)));
        gotoxy(40, 4+i);
        float current = vcc/r1.at(i);
        if(current <= 0.0001) {
            printf("%.3f u\n", current * 1000000);
        } else if(current <= 0.1) {
            printf("%.3f m\n", current * 1000);
        } else {
            printf("%.3f\n", current);
        }
    }
}

float getV2(float vcc, float r1, float r2) {
    return vcc * (r2/(r1+r2));
}

float getR1(float vcc, float v_2, float r2) {
    return (r2 - r2*(v_2 / vcc)) / (v_2 / vcc);
}

float getR2(float vcc, float v_2, float r1) {
    return r1 * (v_2 / vcc) / ( 1 - (v_2/vcc));
}

std::vector<std::pair<unsigned int, unsigned int>> getR1R2(float vcc, float v_2, float i) {
    std::vector<std::pair<unsigned int, unsigned int>> ret;

    float r2=0.0;
    float tmp=0.0;

    for(auto e : e24) {
        r2 = getR2(vcc, v_2, float(e));
        
        tmp = findE24(r2);

        if(i>0.0) {
            if(i < vcc/e) {
                continue;
            }
        }

        if(tmp != -1.0) {
            ret.emplace_back(std::pair<float, float>(e, r2));
        }
    }

    return ret;
}

float findE24(float e) {
    for(auto ex : e24) {
        if((e >= ex*0.9) && (e <= ex*1.1)) {
            return ex;
        }
    }
    return -1.0;
}