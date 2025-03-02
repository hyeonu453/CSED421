// Copyright 2025 Hyeonu-Cho
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

void query1(const char* filename_customer, const char* filename_zonecost);
void query2(const char* filename_lineitem, const char* filename_product);
std::string trim(const std::string& str);

class Table {
 private:
  std::map<std::string, int> column_dict;
  std::vector<std::vector<std::string>> table;
  int idx_cnt;

 public:
  explicit Table(const char* filename) {
    std::ifstream file(filename);
    std::string column_name, value_length;
    std::vector<int> column_length;
    idx_cnt = 0;
    if (!getline(file, column_name) || !getline(file, value_length)) {
      throw std::runtime_error("invalid table");
    }
    for (size_t idx = 0, column_start = 0; idx < value_length.length(); idx++) {
      if (value_length[idx] == ' ' || value_length[idx] == '\0' ||
          value_length[idx] == '\n' || idx == value_length.length() - 1) {
        column_length.push_back(idx - column_start);
        column_dict[trim(
            column_name.substr(column_start, idx - column_start))] = idx_cnt;
        idx_cnt++;
        column_start = idx + 1;
      }
    }
    std::string temp_string;
    while (getline(file, temp_string)) {
      if (temp_string == "") continue;
      std::vector<std::string> temp_vector;
      int idx = 0;
      for (auto n : column_length) {
        temp_vector.push_back(trim(temp_string.substr(idx, n)));
        idx += n + 1;
      }
      table.push_back(temp_vector);
    }
  }

  const std::vector<std::string>& operator[](size_t idx) {
    if (idx >= table.size()) {
      throw std::out_of_range("out of row range");
    }
    return table[idx];
  }

  const std::string& operator[](
      const std::pair<size_t, std::string>& idx) const {
    if (idx.first >= table.size()) {
      throw std::out_of_range("out of row range");
    } else if (column_dict.find(idx.second) == column_dict.end()) {
      throw std::out_of_range("column not found");
    }
    return table[idx.first][column_dict.at(idx.second)];
  }

  size_t size() { return table.size(); }
};

int main(int argc, char** argv) {
  std::string query = argv[1];
  if (query == "q1" && argc == 4) {
    query1(argv[2], argv[3]);
  } else if (query == "q2" && argc == 4) {
    query2(argv[2], argv[3]);
  } else {
    std::cout << "Unknown query" << std::endl;
  }
}

void query1(const char* filename_customer, const char* filename_zonecost) {
  Table customer(filename_customer);
  Table zonecost(filename_zonecost);
  std::set<int> toronto_table;

  for (size_t i = 0; i < zonecost.size(); i++) {
    if (zonecost[{i, "ZONEDESC"}] == "Toronto") {
      toronto_table.insert(std::stoi(zonecost[{i, "ZONEID"}]));
    }
  }

  for (size_t i = 0; i < customer.size(); i++) {
    if (toronto_table.find(std::stoi(customer[{i, "ZONE"}])) !=
            toronto_table.end() &&
        std::stoi(customer[{i, "ACTIVE"}]) == 1) {
      std::cout << customer[{i, "LNAME"}] << std::endl;
    }
  }
}

void query2(const char* filename_lineitem, const char* filename_product) {
  Table lineitem(filename_lineitem);
  Table product(filename_product);
  std::map<int, std::set<std::string>> first_dict;
  std::set<int> second_set;

  for (size_t i = 0; i < lineitem.size(); i++) {
    if (first_dict[std::stoi(lineitem[{i, "BARCODE"}])].find(
            lineitem[{i, "UNAME"}]) ==
        first_dict[std::stoi(lineitem[{i, "BARCODE"}])].end()) {
      if (first_dict[std::stoi(lineitem[{i, "BARCODE"}])].size() > 0) {
        second_set.insert(std::stoi(lineitem[{i, "BARCODE"}]));
      }
      first_dict[std::stoi(lineitem[{i, "BARCODE"}])].insert(
          lineitem[{i, "UNAME"}]);
    }
  }

  for (size_t i = 0; i < product.size(); i++) {
    if (second_set.find(std::stoi(product[{i, "BARCODE"}])) !=
        second_set.end()) {
      std::cout << product[{i, "BARCODE"}] << " " << product[{i, "PRODDESC"}]
                << std::endl;
    }
  }
}

std::string trim(const std::string& str) {
  if (str == "") {
    return str;
  }
  size_t start = 0, end = str.length();
  for (; str[start] == ' '; start++) {}
  for (; str[end - 1] == ' ' && start < end; end--) {}
  return str.substr(start, end - start);
}
