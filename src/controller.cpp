#include "controller.h"

Controller::Controller(Driver *_driver, CredStore *_store, bool _debug)
{
    debug = _debug;
    driver = _driver;
    store = _store;
    server = new WiFiServer(80); //Service Port
}

Controller::~Controller()
{

    delete server;
}

void Controller::init()
{

    // Connect to WiFi network

    // Start the server

    Serial.println("initializing the controller");
    server->begin();

    if (debug)
    {
        Serial.println("Server started");
    }
}

void Controller::parse_request(String request, String *paths, int &n_paths)
{

    MatchState ms;
    char buf[100];
    request.toCharArray(buf, 100);
    ms.Target(buf);
    const char *r_expression = "%/([^%/% ]+)"; 

    if (debug)
    {
        Serial.print("request: ");
        Serial.println(request);
        Serial.print("regexp: ");
        Serial.println(r_expression);
    }

    unsigned int count = 0;
    char temp_buf[100];

    // keep matching until we run out of matches
    for (unsigned int index = 0;
         (ms.Match(r_expression, index) > 0) && (count < max_nr_paths);
         count++)
    {
        // increment index ready for next time, go forwards at least one byte
        index = ms.MatchStart + (ms.MatchLength == 0 ? 1 : ms.MatchLength);
        ms.GetCapture(temp_buf, 0);
        if (debug)
        {
            Serial.print("Captured: ");
            Serial.println(ms.GetCapture(buf, 0));
        }
        paths[count] = temp_buf;
    } // end of for each match

    n_paths = count - 1; // will content of pointer not be lost after this function?
}

String Controller::dispatch_request(String request)
{

    String response;
    String *paths = new String[max_nr_paths];
    int n_paths;
    parse_request(request, paths, n_paths);
    if (paths[0] == "driver")
    {
        response = process_driver_request(paths + 1, n_paths - 1);
    }
    else if (paths[0] == "store")
    {
        response = process_store_request(paths + 1, n_paths - 1);
    }
    delete[] paths;
    return response;
}

void Controller::run()
{

    WiFiClient client = server->available();
    if (!client)
    {
        return;
    }

    Serial.println("new client");
    while (!client.available())
    {
        delay(10);
        Serial.println("waiting for client to be available...");
    }

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    String response = dispatch_request(request);
    client.println(response);
    client.flush();
}

String Controller::process_driver_request(String *paths, int n_paths)
{

    output_request output;
    output.index = paths[0].toInt();
    output.state = (bool)paths[1].toInt();

    driver->set_output(output);

    String html_response =
        "HTTP/1.1 200 OK\n"
        "Content-Type: text/html\n"
        "\n" //  do not forget this one
        "<!DOCTYPE HTML>\n"
        "<html>\n"
        "<p>request received:</p>\n"
        "<p> id" + String(output.index) + ""
        ", state" + String(output.state) + "</p>\n"
        "</html>\n";

    if (debug)
    {
        Serial.println("response:");
        Serial.println(html_response);
    }

    return html_response;
}

String Controller::process_store_request(String *paths, int n_paths)
{
    String response;
    if (paths[0] == "set")
    {
        if (paths[1] == "ssid")
        {
            store->set_ssid(paths[2]);
            response = "set_ssid";
        }
        else if (paths[1] == "pwd")
        {
            store->set_pwd(paths[2]);
            response = "set_pwd";
        }
    }
    else if (paths[0] == "get")
    {
        String buf;
        if (paths[1] == "ssid")
        {
            buf = store->get_ssid();
            response = "get_ssid_" + buf;
        }
        else if (paths[1] == "pwd")
        {
            buf = store->get_pwd();
            response = "get_pwd_" + buf;
        }
        Serial.print("requested data: ");
        Serial.println(buf);
    }

    String html_response =
        "HTTP/1.1 200 OK\n"
        "Content-Type: text/html\n"
        "\n" //  do not forget this one
        "<!DOCTYPE HTML>\n"
        "<html>\n"
        "<p>" + response + "</p>\n"
        "</html>\n";

    if (debug)
    {
        Serial.println("response:");
        Serial.println(html_response);
    }

    return html_response;
}
