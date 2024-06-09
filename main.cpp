#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctime>
#include <map>

bool running = true;
time_t start_time;
std::map<pid_t, char*> background_jobs;

// Signalhandler für SIGINT und SIGCHLD
void handle_signal(int sig) {
    if (sig == SIGINT) {
        std::cout << "\nDo you really want to quit (y/n)? ";
        std::string response;
        std::getline(std::cin, response);
        if (response == "y") {
            running = false;
        }
    } else if (sig == SIGCHLD) {
        // Behandelt beendete Kindprozesse
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            if (background_jobs.find(pid) != background_jobs.end()) {
                std::cout << "\nProcess " << pid << " done (" << background_jobs[pid] << ")" << std::endl;
                background_jobs.erase(pid);
            }
        }
    }
}

// Parsen des Kommandos und Überprüfen, ob es im Hintergrund laufen soll
std::vector<char*> parse_command(std::string& input, bool& run_in_background) {
    if (!input.empty() && input.back() == '&') {
        run_in_background = true;
        input.pop_back();
    } else {
        run_in_background = false;
    }

    std::istringstream iss(input);
    std::vector<char*> args;
    std::string token;
    while (iss >> token) {
        char* arg = new char[token.size() + 1];
        std::strcpy(arg, token.c_str());
        args.push_back(arg);
    }
    args.push_back(nullptr);

    return args;
}

// Starten eines Prozesses mit den angegebenen Argumenten
void execute_command(std::vector<char*>& args, bool run_in_background) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error creating process" << std::endl;
        return;
    } else if (pid == 0) {
        if (execvp(args[0], args.data()) == -1) {
            std::cerr << "Error executing command: ";
            for (int i = 0; i < args.size(); i++) {
                if (args[i] != nullptr) {
                    std::cout << args[i] << " ";
                }
            }
            std::cout << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        if (run_in_background) {
            std::cout << "[" << pid << "]" << std::endl;
            background_jobs[pid] = args[0];
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

int main() {
    // Setzen der Signalhandler
    signal(SIGINT, handle_signal);
    signal(SIGCHLD, handle_signal);
    start_time = time(nullptr);

    std::cout << "Welcome to the shell!" << std::endl;

    while (running) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::cout << cwd << " > ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }

        if (input.empty()) {
            continue;
        }

        bool run_in_background;
        std::vector<char*> args = parse_command(input, run_in_background);

        if (args[0] == std::string("exit")) {
            running = false;
            continue;
        }

        execute_command(args, run_in_background);
    }

    time_t end_time = time(nullptr);
    std::cout << "Shell exited. Runtime: " << difftime(end_time, start_time) << " seconds." << std::endl;

    return 0;
}

