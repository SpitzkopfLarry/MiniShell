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
std::vector<std::string> parse_command(std::string& input, bool& run_in_background) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        if (token == "&") {
            run_in_background = true;
        } else {
            args.push_back(token);
        }
    }
    if (!args.empty() && args.back() == "&") {
        run_in_background = true;
        args.pop_back();
    }
    return args;
}

// Starten eines Prozesses mit den angegebenen Argumenten
void execute_command(const std::vector<std::string>& args, bool run_in_background) {
    std::vector<char*> c_args;
    for (const auto& arg : args) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error creating process" << std::endl;
        return;
    } else if (pid == 0) {
        if (execvp(c_args[0], c_args.data()) == -1) {
            std::cerr << "Error executing command: " << c_args[0] << std::endl;
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
        bool run_in_background = false;
        std::vector<std::string> args = parse_command(input, run_in_background);

        if (args.empty()) {
            continue;
        }

        if (args[0] == "exit") {
            running = false;
            continue;
        }

        execute_command(args, run_in_background);
    }

    time_t end_time = time(nullptr);
    std::cout << "Shell exited. Runtime: " << difftime(end_time, start_time) << " seconds." << std::endl;

    return 0;
}
