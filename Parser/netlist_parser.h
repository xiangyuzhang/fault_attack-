#ifndef PARSER_NETLIST_PARSER_H
#define PARSER_NETLIST_PARSER_H
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <regex>
#include "dict.h"
#include "tools.h"
class netlist_parser_ABC
{
public:
	netlist_parser_ABC() = default;
	netlist_parser_ABC(const std::string);

	virtual ~netlist_parser_ABC();
	virtual void parse_input(std::string);
	virtual void parse_CB(std::string);
	virtual void parse_output(std::string);
	virtual void parse_gate(std::string);

private:
	std::vector<std::string> find_netname(std::string gate);
	std::string find_gatetype(std::string line);
	std::vector<std::string> split_wire_info(std::string line, const std::string  line_type);
	std::string input_file;

	std::map<std::string, unsigned> PI_name_to_index;
	std::map<std::string, unsigned> PO_name_to_index;
	std::map<std::string, unsigned> wire_name_to_index;
	std::map<std::string, unsigned> varIndexDict;

	std::map<unsigned, std::string> PI_index_to_name;
	std::map<unsigned, std::string> PO_index_to_name;
	std::map<unsigned, std::string> wire_index_to_name;
	std::map<unsigned, std::string> indexVarDict;

	std::vector<std::vector<std::string>> CNF;
	std::map<std::string, int> gateTypeDict;
};


netlist_parser_ABC::netlist_parser_ABC(const std::string input):input_file(input)
{
	auto temp = gateTypeDict;
	load_gateTypeDict(temp);
}

void netlist_parser_ABC::parse_gate(std::string gate)
{
	auto gate_type = find_gatetype(gate);
	auto gate_nets = find_netname(gate);
	std::vector<std::string> gate_input(gate_nets.begin(), gate_nets.end() - 2);
	std::string gate_output(gate_nets.back());

	std::vector<int> gate_input_index;
	int gate_output_index = varIndexDict[gate_output];

	for(auto in: gate_input)
	{
		gate_input_index.push_back(varIndexDict[in]);
	}
	strip_all(gate_type, " ");
	strip_all(gate_type, "\t");
	auto caseNO = gateTypeDict[gate];

	auto cnfLines = transGATE(caseNO, gate_input_index, gate_output_index);
	CNF.push_back(cnfLines);
}



std::vector<std::string> netlist_parser_ABC::split_wire_info(std::string line, const std::string line_type)
{
	std::vector<std::string> result;

	strip_all(line, line_type);
	strip_all(line, " ");

	SplitString(line, result, ",");
	for(auto &net: result)
	{
		strip_all(net, "[");
		strip_all(net, "]");
		strip_all(net, " ");
		strip_all(net, "\t");
	}
	return result;
}

std::vector<string> netlist_parser_ABC::find_netname(std::string gate)
{
	std::string s = gate;
	std::vector<string> container;
	std::vector<string> netnames;
	SplitString(s, container, "(");
	for (std::vector<string>::iterator iter = container.begin(); iter != container.end(); ++iter) {
		std::smatch nets;
		std::regex pattern("([^\\)]+)([\\)])");
		std::regex_search(*iter, nets, pattern);
		for (unsigned int i = 0; i < nets.size(); i++) 
		{
			if (nets[i].str().find(")") == string::npos) netnames.push_back(nets[i].str());
		}
	}
	return netnames;
}

std::string netlist_parser_ABC::find_gatetype(std::string line)
{
    strip_all(line, " ");
    std::regex pattern("^([a-z]*)([0-9]*)");
    std::smatch result;
    std::regex_search(line, result, pattern);
    return result[1].str();
}
#endif