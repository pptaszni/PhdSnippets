#include <fstream>
#include <iostream>
#include <set>

#include <nlohmann/json.hpp>

struct VbPlayer
{
  std::string name;
  std::string surname;
  std::string phone;
};

bool operator<(const VbPlayer& left, const VbPlayer& right)
{
  if (left.surname == right.surname) return left.name < right.name;
  return left.surname < right.surname;
}

std::ostream& operator<<(std::ostream& os, const VbPlayer& elem)
{
  os << "VbPlayer{ " << elem.name << " " << elem.surname << " " << elem.phone << " }";
  return os;
}

int main(int argc, char** argv) {
  if (argc < 3) return -1;
  nlohmann::json data;
  nlohmann::json list;
  std::ifstream playersData(argv[1]);
  std::ifstream playersList(argv[2]);
  playersData >> data;
  playersList >> list;
  std::set<VbPlayer> playersDataSet;
  std::set<VbPlayer> playersListSet;
  for (const auto& elem : data["players"])
  {
    playersDataSet.insert({elem["name"].get<std::string>(),
      elem["surname"].get<std::string>(), elem["phone"].get<std::string>()});
  }
  for (const auto& elem : list["players"])
  {
    playersListSet.insert({elem["name"].get<std::string>(),
      elem["surname"].get<std::string>(), "___"});
  }
  for (const auto& elem : playersListSet)
  {
    auto found = playersDataSet.find(elem);
    if (found == playersDataSet.end()) std::cout << elem << std::endl;
    else std::cout << *found << std::endl;
  }
  return 0;
}
