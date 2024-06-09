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
std::map<pid_t, std::string> background_jobs;

void handle_signal(int sig);
void handle_signal(int sig) {
    if (sig == SIGINT) {
        std::cout << "\nDo you really want to quit (y/n)? ";
        std::cout.flush();
        std::string response;
        std::getline(std::cin, response);
        if (response == "y") {
            running = false;
        } else {
            std::cout << std::endl;
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
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    std::vector<char*> args;
    for (auto& t : tokens) {
        args.push_back(&t[0]);
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
            std::cerr << "Unknown command: " << args[0] << std::endl;
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

void setup_signal_handlers() {
    struct sigaction sa_int;
    struct sigaction sa_chld;

    sa_int.sa_handler = handle_signal;
    sa_chld.sa_handler = handle_signal;

    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_chld.sa_mask);

    sa_int.sa_flags = 0;
    sa_chld.sa_flags = SA_RESTART;  // Restart functions if interrupted by handler

    sigaction(SIGINT, &sa_int, nullptr);
    sigaction(SIGCHLD, &sa_chld, nullptr);
}

int main() {
    setup_signal_handlers();
    start_time = time(nullptr);

    std::cout << "Welcome to the shell!" << std::endl;

    while (running) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::cout << cwd << " > ";
        std::cout.flush();

        std::string input;
        if (!std::getline(std::cin, input)) {
            if (!running) {
                break;
            }
            continue;
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
