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
            // v{name}: db {value}
            dataL64 = dataL64 + tab + "v" + std::to_string(counter) + ": db " + instruction.second + "\n";
            // c{name}: equ $ - {prename}
            dataL64 = dataL64 + tab + "c" + std::to_string(counter) + ": equ $ - " + "v" + std::to_string(counter) + "\n";
            // mov rax,1
            // mov rdi,1
            // mov rsi,name
            // mov rdx,namelen
            // syscall
            if (counter != 0) {
                auto past_instruction = sp(og[counter - 1]);
                if (past_instruction.first != "raw_log" and past_instruction.first != "log") {
                    textL64 = textL64 + tab + "mov rax,1\n";
                    textL64 = textL64 + tab + "mov rdi,1\n";
                }
            }
            if (counter == 0) {
                textL64 = textL64 + tab + "mov rax,1\n";
                textL64 = textL64 + tab + "mov rdi,1\n";
            }
            textL64 = textL64 + tab + "mov rsi,v" + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "mov rdx,c" + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "syscall\n";
            // linux arm
            // v{cuount}:	.asciz {value}
            dataA64 = dataA64 + tab + "v" + std::to_string(counter) + ": .asciz " + instruction.second + "\n";
            //mov	x0, #1
            //ldr	x1, =v{cuount}
            //mov	x2, #3
            //mov	x8, #64
            //svc	#0 syscall
            textA64 = textA64 + tab + "mov	x0, #1\n";
            textA64 = textA64 + tab + "ldr	x1, =v" + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "mov	x2, #3\n";
            textA64 = textA64 + tab + "mov	x8, #64\n";
            textA64 = textA64 + tab + "svc	#0\n";

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
    return 0;
}
