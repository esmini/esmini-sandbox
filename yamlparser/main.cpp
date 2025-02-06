
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <fstream>
#include <iostream>
#include <vector>

void ParseYamlFile(const std::string& filename)
 {
    std::ifstream file(filename, std::ios::ate);
    if (!file) 
	{
        std::cerr << "Failed to open YAML file!" << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> yaml_data(size + 1, '\0');  // Null-terminated
    file.read(yaml_data.data(), size);

    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(yaml_data));
    std::string name;
    std::string age;
    std::string skill;
    tree["name"] >> name;
    tree["age"] >> age;
    std::cout << "Name: " << name << "\n";
    std::cout << "age: " << age << "\n";
    ryml::NodeRef skills_node = tree["skills"];
    for (size_t i = 0; i < skills_node.num_children(); ++i) 
    {
        skills_node[i] >> skill;
        std::cout << "skill: " << skill << "\n";
    }
}

void CreateConfigFile(const std::string& filename)
{
	std::ofstream file(filename);
	if (!file)
	{
	  std::cerr << "Failed to create file: " << filename << std::endl;
	  return;
	}
	
	file << "name: Mike Ross \n";
	file << "age: 25\n";
	file << "skills:\n";
	file << " - drama\n";
	file << " - writing";

	file.close();
	std::cout << "config file created." << std::endl;
}

int main()
{
	std::cout << "yaml parser 0.1" << std::endl;
	CreateConfigFile("config.yml");
	ParseYamlFile("config.yml");	
}