#include <iostream>
#include <string>
#include <map>

using namespace std;


class Logger
{
    public:
        Logger() {};
        void Log(string payload)
        {
            cout << payload << endl;
        }
};


class Multiplier
{
    private:
        float factor;
    public:
        Multiplier(float _factor=0): factor(_factor) {};
        float Multiply(float input)
        {
            cout << "Performing multiplication..." << endl;
            return input * factor;
        }
        float Multiply(string input)
        {
            cout << "Performing multiplication..." << endl;
            return stof(input) * factor;
        }
};



class Dispatcher
{
    public:
        virtual void dispatch(string payload) = 0;
};


struct Rule {
    string identifier;
    Dispatcher* Dispatcher;
};


class Command
{
    public:
        virtual string execute(string Payload = "") = 0;
};


class Path_Dispatcher : public Dispatcher
{
    private:
        Rule* rules;
        char delimiter;
        int rule_count;
        const int max_rules = 10;
    public:
        Path_Dispatcher(char _delimiter = '/')
        {
            delimiter = _delimiter;
            rules = new Rule[max_rules];
            rule_count = 0;
        };
        ~Path_Dispatcher()
        {
            delete[] rules;
        };
        void add_rule(Rule rule)
        {
            if (rule_count < max_rules)
            {
                rules[rule_count] = rule;
                rule_count++;
            }
        };
        void dispatch(string payload) override
        {
            cout << "executing dispatcher on payload:" << payload << endl;
            string token1, token2;
            int delimiter_position = payload.find(delimiter);

            if (delimiter_position == string::npos)
            {
                cout << "No delimeter found" << endl;
                return;
            }

            token1 = payload.substr(0, delimiter_position);
            token2 = payload.substr(delimiter_position + 1);

            cout << "Token 1:" << token1 << endl;
            cout << "Token 2:" << token2 << endl;            


            for (int i=0; i < rule_count; i++)
            {
                cout << "Dispatching... token:" << token1 << ", identifier: " << rules[i].identifier << endl;
                if (token1 == rules[i].identifier)
                {

                    rules[i].Dispatcher->dispatch(token2);
                    break;
                }
            }

        }
};


class Command_Dispatcher : public Dispatcher
{
    private:
        Command* command;
    public:
        Command_Dispatcher(Command& _command): command(&_command) {};
        void dispatch(string payload) override
        {
            command->execute(payload);
        }
};



class ExecutionBehavior
{
    public:
        virtual string apply(Command** commands, int amount, string payload) = 0;
};



class StackedExecutionBehavior: public ExecutionBehavior
{
    public:
        string apply(Command** commands, int amount, string payload) override
        {
            string old_result = payload;
            string updated_result;
            for (int i=0; i<amount; i++)
            {
                cout << "Applying stacked execution strategy, step " << i << endl;                
                updated_result = commands[i]->execute(old_result);
                old_result = updated_result;
            };
            return updated_result;
        }

};


class SequentialExecutionBehavior: public ExecutionBehavior
{
    public:
        string apply(Command** commands, int amount, string payload) override
        {
            string result;
            for (int i=0; i<amount; i++)
            {
                cout << "Applying sequential execution strategy, step " << i << endl;                
                result = commands[i]->execute(payload);
            };
            return result;
        }

};



class Macro_Command : public Command
{
    private:
        Command *commands[10];
        int amount;
        ExecutionBehavior* behavior;
    public:
        Macro_Command(ExecutionBehavior& b): behavior(&b) {amount=0;};
        ~Macro_Command() {};
        void add_command(Command& c)
        {
            commands[amount] = &c;
            amount ++;
        }
        string execute(string payload = "") override
        {
            return behavior->apply(commands, amount, payload);
        };
};




class Multiply_Command : public Command
{
    private:
        Multiplier* multiplier;
    public:
        Multiply_Command(Multiplier& _multiplier): multiplier(&_multiplier) {};
        ~Multiply_Command() {};
        string execute(string Payload = "") override
        {
            string result;
            result = to_string(multiplier->Multiply(Payload));
            return result;
        };
};



class Write_Payload : public Command
{
    private:
        Logger* logger;
    public:
        Write_Payload(Logger& _logger)
        {
            logger = &_logger;
        }
        ~Write_Payload() {};
        string execute(string Payload) override
        {
            logger->Log("Payload is: " + Payload);
            return "";
        };
};


class Write_Payload_Funny : public Command
{
    private:
        Logger* logger;
    public:
        Write_Payload_Funny(Logger& _logger)
        {
            logger = &_logger;
        }
        ~Write_Payload_Funny() {};
        string execute(string Payload) override
        {
            logger->Log("Funny Payload is: " + Payload);
            return "";            
        };
};


class Say_Hello : public Command
{
    private:
        Logger* logger;
    public:
        Say_Hello(Logger& _logger): logger(&_logger) {};
        ~Say_Hello() {};
        string execute(string Payload = "") override
        {
            logger->Log("Hello without payload");
            return "";
        };
};



class Rule_Factory
{
    public:
        Rule create(string identifier, Dispatcher* dispatcher) {
            Rule rule;
            rule.identifier = identifier;
            rule.Dispatcher = dispatcher;
            return rule;
        };
};


class Dispatcher_Factory
{

};


int main() {




    std::map<char, char> my_map = {
        { 'A', '1' },
        { 'B', '2' },
        { 'C', '3' }
    };




    Logger logger = Logger();
    Multiplier m = Multiplier(100);
    Rule_Factory rule_factory = Rule_Factory();    
    StackedExecutionBehavior steb = StackedExecutionBehavior();
    SequentialExecutionBehavior seeb = SequentialExecutionBehavior();
    Say_Hello say_hello = Say_Hello(logger);
    Write_Payload write_payload = Write_Payload(logger);
    Write_Payload_Funny write_payload_funny = Write_Payload_Funny(logger);
    Multiply_Command multiply = Multiply_Command(m);
    Macro_Command multiply_and_write = Macro_Command(steb);
    multiply_and_write.add_command(multiply);
    multiply_and_write.add_command(write_payload_funny);
    
    Path_Dispatcher dispatcher1 = Path_Dispatcher();
    Path_Dispatcher dispatcher2 = Path_Dispatcher();
    Command_Dispatcher endpoint1 = Command_Dispatcher(write_payload);
    Command_Dispatcher endpoint2 = Command_Dispatcher(write_payload_funny);
    Command_Dispatcher endpoint3 = Command_Dispatcher(say_hello);
    Command_Dispatcher endpoint4 = Command_Dispatcher(multiply_and_write);
    
    dispatcher1.add_rule(rule_factory.create("GF", &dispatcher2));
    dispatcher1.add_rule(rule_factory.create("WP", &endpoint1));
    dispatcher1.add_rule(rule_factory.create("WPF", &endpoint2));
    dispatcher1.add_rule(rule_factory.create("SH", &endpoint3));

    dispatcher2.add_rule(rule_factory.create("WP2", &endpoint1));
    dispatcher2.add_rule(rule_factory.create("WPF2", &endpoint2));
    dispatcher2.add_rule(rule_factory.create("SH2", &endpoint3));
    dispatcher2.add_rule(rule_factory.create("MN2", &endpoint4));

    string s = "WPF/qwerty";
    dispatcher1.dispatch(s);
    cout << "----------------" << endl;
    s = "GF/SH2/qwerty";
    dispatcher1.dispatch(s);
    cout << "----------------" << endl;
    s = "GF/MN2/10";
    dispatcher1.dispatch(s);










    
}