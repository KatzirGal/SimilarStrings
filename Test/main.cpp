#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#include <Poco/Exception.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>

int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        std::cerr << "Error: Missing argument!" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Error: File does not exist!" << std::endl;
        return 1;
    }

    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return 1;
    }

    std::vector<std::string> words;
    std::string line;
    while (std::getline(file, line))
    {
        words.push_back(line);
    }

    std::random_device rd;  // Seed source
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<size_t> dist(0, words.size() - 1);

    try {
        std::vector<std::thread> threads;

        const int num_threads = std::thread::hardware_concurrency();
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&dist, &gen, &words]() {
                for (int i = 0; i < 30000; ++i)
                {
                    size_t random_index = dist(gen);

                    Poco::Net::HTTPClientSession session("127.0.0.1", 8000);

                    std::string reqUrl = std::format("/api/v1/similar?word={}", words[random_index]);

                    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, reqUrl);
                    session.sendRequest(request);

                    Poco::Net::HTTPResponse response;
                    std::istream& responseStream = session.receiveResponse(response);
                    if (response.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
                    {
                        std::cout << "Request '" << reqUrl << "' ended with error: " << response.getReason() << std::endl;
#ifdef _WIN32
                        MessageBoxA(NULL,           // Handle to the owner window (NULL for no owner)
                            response.getReason().c_str(),// Message text
                            "Message Box",  // Message box title
                            MB_OK | MB_ICONINFORMATION); // Buttons and icon
#endif
                    }
                    else
                    {
                        std::cout << "Request '" << reqUrl << "' ended with success" << std::endl;
                        std::cout << "Response: " << response.getStatus() << " " << response.getReason() << std::endl;

                        Poco::StreamCopier::copyStream(responseStream, std::cout);
                        std::cout << std::endl;
                    }
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        Poco::Net::HTTPClientSession session("127.0.0.1", 8000);

        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/api/v1/stats");
        session.sendRequest(request);

        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse(response);

        // Read response into a stringstream
        std::stringstream ss;
        Poco::StreamCopier::copyStream(responseStream, ss);
        std::string responseStr = ss.str();

        std::cout << std::endl;
        std::cout << std::flush;

#ifdef _WIN32
        MessageBoxA(NULL,           // Handle to the owner window (NULL for no owner)
            responseStr.c_str(),// Message text
            "Message Box",  // Message box title
            MB_OK | MB_ICONINFORMATION); // Buttons and icon
#endif
        return 0;
        
    }
    catch (Poco::Exception& ex) {
        std::cerr << "Exception: " << ex.displayText() << std::endl;
    }

    return 0;
}

