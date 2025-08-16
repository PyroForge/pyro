#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
std::pair<std::string, std::string> sp(const std::string& input) {
    size_t colonPos = input.find(':');
    size_t semicolonPos = input.find(';');
    size_t splitPos = std::string::npos;
    if (colonPos != std::string::npos && semicolonPos != std::string::npos) {
        splitPos = std::min(colonPos, semicolonPos);
    } else if (colonPos != std::string::npos) {
        splitPos = colonPos;
    } else if (semicolonPos != std::string::npos) {
        splitPos = semicolonPos;
    }
    if (splitPos != std::string::npos) {
        std::string before = input.substr(0, splitPos);
        std::string after = input.substr(splitPos + 1);
        return {before, after};
    } else {
        // No delimiter found: return whole string as "before", empty string as "after"
        return {input, ""};
    }
}
std::pair<int, int> get_b_count(const std::vector<std::string>& og, int lnum, int bback) {
    int armbytes = 0;
    int x86bytes = 0;

    const int rl_x86 = 30, rl_arm = 5;
    const int sc_x86 = 10, sc_arm = 1;
    const int ed_x86 = 20, ed_arm = 3;

    // Loop over the last `bback` lines before `lnum`
    for (int i = lnum - bback; i < lnum; ++i) {
        if (i < 0 || i >= static_cast<int>(og.size())) continue;

        std::pair<std::string, std::string> parts = sp(og[i]);
        std::string first = parts.first;
        std::string second = parts.second;

        if (first == "raw_log") {
            x86bytes += rl_x86;
            armbytes += rl_arm;
        } else if (first == "syscall") {
            x86bytes += sc_x86;
            armbytes += sc_arm;
        } else if (first == "end") {
            x86bytes += ed_x86;
            armbytes += ed_arm;
        }
    }

    return {x86bytes, armbytes};
}
int main(int argc, char* argv[]) {
    // config
    std::string output = argv[2];
    std::string tab = "    ";


    //set up vars
    // linux 64bit x86_64 nasm
    std::string topL64 = ";";
    std::string bssL64 = "section .bss\n";
    std::string dataL64 = "section .data\n";
    std::string textL64 = "section .text\n" + tab + "global _start\n_start:\n";
    // linux 64bit arm
    std::string topA64 = ";";
    std::string bssA64 = ".section .bss\n";
    std::string dataA64 = ".section .data\n";
    std::string textA64 = ".section .text\n" + tab + ".global _start\n_start:\n";

    // open file and puting it in list
    std::ifstream file(argv[1]);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string preog = buffer.str();
    file.close();
    std::istringstream iss(preog);
    std::string linee;
    std::vector<std::string> og;
    while (std::getline(iss, linee)) {
        og.push_back(linee);
    }

    int counter = 0;
    
    while (counter < og.size()) {
        auto instruction = sp(og[counter]);
        if (instruction.first == "end") {
            // linux
            textL64 = textL64 + tab + "mov rax,60\n";
            textL64 = textL64 + tab + "xor rdi,rdi\n";
            textL64 = textL64 + tab + "syscall\n";
            // linux arm
            textA64 = textA64 + tab + "mov	x0, #0\n";
            textA64 = textA64 + tab + "mov	x8, #93\n";
            textA64 = textA64 + tab + "svc	#0\n";
        }
        if (instruction.first == "syscall") {
            // linux
            textL64 = textL64 + tab + "syscall\n";
            // linux arm
            textA64 = textA64 + tab + "svc	#0\n";
        }
        if (instruction.first == "raw_log") {
            // linux
            dataL64 = dataL64 + tab + "v" + std::to_string(counter) + ": db " + instruction.second + "\n";
            dataL64 = dataL64 + tab + "c" + std::to_string(counter) + ": equ $ - " + "v" + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "mov rax,1\n";
            textL64 = textL64 + tab + "mov rdi,1\n";
            textL64 = textL64 + tab + "mov rsi,v" + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "mov rdx,c" + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "syscall\n";
            // linux arm
            dataA64 = dataA64 + tab + "v" + std::to_string(counter) + ": .asciz " + instruction.second + "\n";
            dataA64 = dataA64 + tab + "c" + std::to_string(counter) + " = . - " + "v" + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "mov	x8, #64\n";
            textA64 = textA64 + tab + "mov	x0, #1\n";
            textA64 = textA64 + tab + "ldr	x1, =v" + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "mov	x2, c" + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "svc	#0\n";
        }
        if (instruction.first == "back") {
            int backc = std::stoi(instruction.second);
            std::pair<int, int> result = get_b_count(og, counter, backc);
            int x86bytes = result.first;
            int armbytes = result.second;
            if (x86bytes > 2147483647 or armbytes > 134217727) {
                std::cout << "Line " << std::to_string(counter) << "\nCan not jump back more the 134217727 bytes or ~19173961 pyro lines.\nps. question what you're doing bc that a huge jump\nExiting...\n";
                return EXIT_FAILURE;
            }
            //linux
            textL64 = textL64 + tab + "jmp near -" + std::to_string(x86bytes) + "\n";
            //linux arm
            textA64 = textA64 + tab + "b #-" + std::to_string(armbytes) + "\n";
        }
        counter++;

        
    }

    std::ofstream L64(output + "linux_86x_64_pyro.asm");
    L64 << topL64 + "\n";
    L64 << bssL64 + "\n";
    L64 << dataL64 + "\n";
    L64 << textL64 + "\n";
    L64.close();
    std::ofstream A64(output + "linux_arm_pyro.s");
    A64 << topA64 + "\n";
    A64 << bssA64 + "\n";
    A64 << dataA64 + "\n";
    A64 << textA64 + "\n";
    A64.close();
    return EXIT_SUCCESS;
}
