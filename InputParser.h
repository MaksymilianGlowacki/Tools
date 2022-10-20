//
// Created by Maksymilian Głowacki on 19/10/2022.
//

#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <map>

#ifndef TOOLS_INPUTPARSER_H
#define TOOLS_INPUTPARSER_H

namespace tools
{

    class FlagOpt
    {
    public:
        FlagOpt(std::string short_name, std::string long_name, std::string helper_text, bool isOpt = false,
                bool isArg = false) :
                isOpt(isOpt), isArg(isArg), short_name(std::move(short_name)),
                long_name(std::move(long_name)), helper_text(std::move(helper_text)) {}

        bool operator==(const FlagOpt& f) const
        {
            return ((short_name == f.short_name) || (long_name == f.long_name));
        }
        [[nodiscard]] std::string get_short_name() const {return short_name;}
        [[nodiscard]] std::string get_long_name() const {return long_name;}
        [[nodiscard]] std::string get_help() const {return helper_text;}
        const bool isOpt, isArg;

        struct HashFunction
        {
            size_t operator()(const FlagOpt& f) const
            {
                size_t xHash = std::hash<std::string>()(f.short_name);
                return xHash;
            }
        };

    private:
        const std::string short_name, long_name, helper_text;
    };


    class InputParser
    {
    public:
        InputParser(int argc, char *argv[])
        {
            for(int i = 0; i < argc; ++i)
            {
                input.emplace_back(argv[i]);
            }
            add_flag("h", "help", "Show this help message");
        }

        void add_option(std::string short_name, std::string long_name, std::string helper_text)
        {
            auto opt = FlagOpt(std::move(short_name), std::move(long_name),
                               std::move(helper_text), true);
            if(flags_opts.count(opt))
            {
                std::cerr << "Flag/option/argument with the same name/long name: \"-" << opt.get_short_name() << ", --"
                    << opt.get_long_name() << "\" already exists!\n";
            } else {
                flags_opts.insert(opt);
            }
        };

        void add_argument(std::string short_name, std::string long_name, std::string helper_text)
        {
            auto arg = FlagOpt(std::move(short_name), std::move(long_name),
                              std::move(helper_text), true, true);
            if(flags_opts.count(arg))
            {
                std::cerr << "Flag/option/argument with the same name/long name: \"-" << arg.get_short_name() << ", --"
                          << arg.get_long_name() << "\" already exists!\n";
            } else {
                flags_opts.insert(arg);
                args.emplace_back(arg);
            }
        };

        void add_flag(std::string short_name, std::string long_name, std::string helper_text)
        {
            auto f = FlagOpt(std::move(short_name), std::move(long_name),
                             std::move(helper_text));
            if(flags_opts.count(f))
            {
                std::cerr << "Flag/option/argument with the same name/long name: \"-" << f.get_short_name() << ", --"
                          << f.get_long_name() << "\" already exists!\n";
            } else {
                flags_opts.insert(f);
            }
        };

        std::map<std::string, std::string> parse()
        {
            unsigned long arg_count = 0;
            if(input.size() < 2)
            {
                printHelp();
            }
            for(unsigned long i = 1; i < input.size(); ++i)
            {
                const auto& str = input[i];
                if(str == "-" || str == "--")
                {
                    std::cerr << "Unrecognised option/flag: " << str << " !\n\n";
                    exit(1);
                }
                if(str[0] == '-' && str[1] == '-')
                {
                    auto fopt = FlagOpt("", str.substr(2), "");
                    auto it = flags_opts.find(fopt);
                    if(it == flags_opts.end() || it->isArg)
                    {
                        std::cerr << "Unrecognised option/flag: " << str << " !\n\n";
                        exit(1);
                    } else if(it->isOpt)
                    {
                        if(input.size() - i > 1)
                        {
                            result[it->get_short_name()] = input[++i];
                        } else
                        {
                            std::cerr << "No value for option: " << str << " !\n\n";
                            exit(1);
                        }
                    } else
                    {
                        result[it->get_short_name()] = "";
                    }
                } else if(str[0] == '-')
                {
                    for(const auto j : str.substr(1))
                    {
                        auto fopt = FlagOpt(std::string(1, j), "", "");
                        auto it = flags_opts.find(fopt);
                        if(it == flags_opts.end() || it->isArg)
                        {
                            std::cerr << "Unrecognised option/flag: " << str << " !\n\n";
                            exit(1);
                        }
                        if(it->isOpt)
                        {
                            if(input.size() - i > 1)
                            {
                                result[it->get_short_name()] = input[++i];
                            } else
                            {
                                std::cerr << "No value for option: " << str << " !\n\n";
                                exit(1);
                            }
                        } else
                        {
                            result[it->get_short_name()] = "";
                        }
                    }
                } else
                {
                    if(arg_count >= args.size())
                    {
                        std::cerr << "Value: " << str << " has no matching argument!\n\n";
                        exit(1);
                    } else
                    {
                        result[args[arg_count++].get_short_name()] = str;
                    }
                }
            }
            if(result.count("h")) printHelp();
            if(arg_count != args.size())
            {
                std::cerr << "Too few arguments!\n\n";
                exit(1);
            }
            return result;
        }
        std::map<std::string, std::string> result;
        void printHelp()
        {
            std::cout << "Usage: " << input[0] << " [flags/options] ";
            for(const auto& arg : args)
            {
                std::cout << arg.get_long_name() << ' ';
            } std::cout << '\n';
            for(const auto& i : args)
            {
                std::cout << "\t\t" << i.get_short_name() << ", " << i.get_long_name() << "\t\t"
                          << i.get_help() << '\n';
            } std::cout << "Flags/options:\n";
            for(const auto& i : flags_opts)
            {
                if(!i.isArg)
                {
                    if(i.isOpt)
                    {
                        std::cout << "\t\t-" << i.get_short_name() << ", --" << i.get_long_name() << "\tval\t"
                                  << i.get_help() << '\n';
                    } else
                    {
                        std::cout << "\t\t-" << i.get_short_name() << ", --" << i.get_long_name() << "\t\t"
                                  << i.get_help() << '\n';
                    }
                }
            }
            exit(0);
        }
    private:
        std::vector<std::string> input;
        std::unordered_set<FlagOpt, FlagOpt::HashFunction> flags_opts;
        std::vector<FlagOpt> args;
    };
}


#endif //TOOLS_INPUTPARSER_H
