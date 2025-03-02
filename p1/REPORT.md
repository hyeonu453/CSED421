# WHY DBMS? Report

Name: Hyeonu Cho  
Student id: 20230740

# Problem Analysis

WHY DBMS? 프로젝트는 후술할할 2개의 Query를 구현하는 것을 목표로 한다.  
본 프로젝트는 Ubuntu 24.04.2 및 g++ 13.3.0 환경에서 구현되었음을 밝힌다.
## Query 1

Query 1는 사람들의 다양한 개인정보가 담긴 `customer` 테이블과 지역에 대한 정보가 있는 `zonecost`테이블을 사용하여, `customer` 테이블에 포함된 사람 중 Toronto에 살며 ACTIVE인 사람의 Last name을 모두 출력하는 것을 목표로 한다.  

이때, `customer` 테이블은 여러 Column을 가지는데 본 Project에서는 다음의 Column을 사용한다.
- `LNAME` : 해당하는 사람의 Last Name을 출력하기 위해 사용된다.
- `ZONE` : `zonecost` 테이블의 Column `ZONEID`에 해당하는 Column으로, Toronto에 해당하는 `ZONE`을 찾기 위해 사용된다.
- `ACTIVE` : 1이면 ACTIVE, 0이면 아님을 나타내며 ACTIVE 여부를 확인하기 위해 사용된다.

`zonecost` 테이블의 Column 중에서 본 프로젝트에서는 다음의 Column을 사용한다.
- `ZONEID` : 상술한 바와 같이, `customer` 테이블의 `ZONE`에 해당하며 해당 정보를 바탕으로 Toronto에 살고있는 사람을 찾는다. 
- `ZONEDESC` : `ZONEID`의 해당하는 지역에 대한 정보, 즉 지명을 저장하며 Toronto를 찾기 위해 사용된다.

따라서, Query 1은 다음의 과정을 거쳐 해결될 수 있다.  
1. `zonecost` 테이블의 row들 중 Column `ZONEDESC`가 Toronto인 row의 `ZONEID`를 모두 테이블에 저장한다. 
2. `customer` 테이블의 row중 Column `ZONE`이 위에서 저장한 `ZONEID`들이 담긴 테이블에 포함되어 있다면, 이를 출력한다.

## Query 2

Query 2는 손님들의 물품 구매 정보가 담긴 `lineitem` 테이블과 물품품에 대한 정보가 있는 `products`테이블을 사용하여, `products` 테이블에 포함되어있는 물품 중 `lineitem` 테이블에 포함된 손님들 중 적어도 2명 이상이 구매한 물품을 모두 출력하는 것을 목표로 한다.  

이때, `lineitem` 테이블은 여러 Column을 가지는데 본 Project에서는 다음의 Column을 사용한다.
- `UNAME` : 손님의 이름을 저장하며, 손님마다 유니크한 값을 가지므로 임의의 물품이 여러 손님에 의해 구매되었는지 확인하기위해 사용된다.
- `BARCODE` : 물품별로 할당되는 고유한 값으로 물품의 이름을 불러오는 데에 사용된다.

`products` 테이블의 Column 중에서 본 프로젝트에서는 다음의 Column을 사용한다.
- `BARCODE` : 상술한 바와 같이 물품의 이름을 불러오는 데에 사용된다.
- `PRODDESC`: 물품의 정보, 즉 이름을 저장하여 불러오는 데에 사용된다.

따라서 Query 2는 다음의 과정을 거쳐 해결될 수 있다.
1. `lineitem` 테이블의 손님들이 구매한 물품에 대해, 각 상품별로 어떤 손님이 구매했는지 저장하는 테이블을 만든다.
2. 이때, 한 품목을 여러 손님이 구매한 경우 해당 품목들을 다른 테이블에 저장한다.
3. 이후 해당 품목들의 정보를 `products` 테이블에서 찾아 `BARCODE`와 `PRODDESC`에 해당하는 정보를 출력한다.

# Design For Problem Solving

## High Level

본 프로젝트는 크게 `Class Table`, `void query()` 이 두 부분으로 구분되어 구성될 것이다. 

### `Class Table`

본 클래스는 `'filename'.txt`로 들어온 테이블에 대해 이를 직접 접근 가능한 테이블의 형태로 구성하는 것을 목표로 하여 작성되었다.

해당 클래스는 `'tablename'[idx]`로 idx번째 row에 접근하거나, `'tablename'[idx]['columnname']`로 idx번째 row의 'columnname'의 해당하는 값을 문자열 형태로 가져올 수 있도록 디자인 하였다. 

이때 사용 및 구현의 용이성을 위해 Column은 `std::string`으로 접근하며, 그리고 각 값은 `std::string`인 문자열로 반환하도록 디자인 하였다. 


### `void query()`

Query 함수는 총 2개, `void query1()`과 `void query2()`로 구성되어 있으며 각각 상술한 과정을 후술할 내용으로 코드로 구현하였다. 

## Low Level

### `Class Table`

해당 클래스는 다음과 같은 private 멤버 변수를 가진다.
- `std::map<std::string, int> column_dict` : 문자열로 column에 접근할 수 있도록 해주는 `std::map`으로, column 이름을 입력하면 해당하는 column이 위치한 index를 반환한다. 
- `std::vector<std::vector<std::string>> table` : `std::string`을 값으로 가지는 2차원 `std::vector`로, Table의 모든 값을 저장한다.
- `int idx_cnt` : Column의 개수를 저장한다. 

다음과 같은 public 멤버 메소드를 가진다. 
- `explicit Table(const char* filename)` : 생성자로, `'filename.txt`를 매개변수로 받는다.
    1. 해당파일의 첫번째 줄로부터 Column의 이름 및 순서를, 두번째 줄로부터 각 Column 값(문자열)의 최대 길이와, 개수를 불러와 적절히 저장한다.
    2. 이후 한줄씩 불러와서 각 row에 해당하는 Vector의 값을 Column 순서에 맞춰서 저장한다.
- `const std::vector<std::string>& operator[](size_t idx)` : idx번째 row를 `std::vector`의 형태로 반환한다.
- `const std::string& operator[](const std::pair<size_t, std::string>& idx) const` : idx번째 row, 'columnname'의 Column에 해당하는 값을 문자열로 반환한다.

### `void query()`
다음은 각 두 Query 함수에 대한 설명이다.
#### `void query1(const char* filename_customer, const char* filename_zonecost)`
필요한 두 테이블을 파일로써 받아온 후, 이를 통해 각각 `Class Table`을 생성한다.   
그리고 Toronto에 해당할 ZoneID를 저장할 `std::set<int> toronto_table`을 하나 추가로 선언한다.

이후, `zonecost` Table에 대해 `ZONEDESC`가 "Toronto"인 것에대해 상술한 `toronto_table`에 `ZONEID`를 추가한다.

그 이후, `customer` Table에 대해 `ZONE`이 `toronto_table`에 존재하며 `ACTIVE`가 1인 경우의 `LNAME`을 출력한다.

#### `void query2(const char* filename_lineitem, const char* filename_product)`
필요한 두 테이블을 파일로써 받아온 후, 이를 통해 각각 `Class Table`을 생성한다.   
그리고 `BARCODE` 별로 구매한 손님을 저장할 `std::map<int, std::set<std::string>> first_dict`을 선언하고, 출력할 목록에 해당하는 `BARCODE`를 저장할 `std::set<int> second_set`을 선언한다.

이후, `lineitem` Table에 대해, `first_map`의 `BARCODE`에 해당하는 set에 `UNAME`에 해당하는 사람이 없다면 이를 추가한다. 그 과정에서, 다른 사람이 존재한다면 해당 `BARCODE`를 `second_set`에 추가한다.

그 이후, `product` Table에 대해 `BARCODE`가 `second_set`에 존재하는 경우의 `BARCODE`와 `PRODDESC`을을 출력한다.
# Mapping Between Implementation And the Design

본 부분에서는 함수 및 메소드의 구현에 대해서 설명하고 있으며, 이를 위해 필요한 변수 등은 대부분 생략되었다.

>본 구현을 위해 추가로 구현한 함수 `std::string trim(const std::string& str)`이 존재한다.
>이는 다음과 같이 구현되어있다. 
>```cpp
>std::string trim(const std::string& str) {
>  if (str == "") {
>    return str;
>  }
>  size_t start = 0, end = str.length();
>  for (; str[start] == ' '; start++) {}
>  for (; str[end - 1] == ' ' && start < end; end--) {}
>  return str.substr(start, end - start);
>}
>```
>해당 함수는 매개변수로 들어온 `str`에 대해 앞뒤의 공백을 모두 없앤 문자열을 반환한다.

`Class Table`은 다음과 같이 구현되어 있다.
```cpp
class Table {
 ...
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
```
본 구현에서는 다음의 특징을 보인다.
- Column Name 및 길이를 불러오는 과정에서 두번째 줄의 '-' 문자가 해당 부분의 길이를 나타냄을 이용하여 동일한 인덱스의 `trim()`결과를 Column이름으로, 그리고 각 Column의 Offset을 `std::vector<int> column_length`에 저장하였다. 
- 이후, 각 오프셋에 맞춰서 문자를 불러오고 `trim()`한 결과 및 인덱스에 맞춰 `std::vector<std::string> temp_vector`에 저장하였으며 이를 `table`에 추가하는 방식으로 table을 구성하였다.
- 접근할 수 없는 인덱스의 경우 Exception throwing을 통해 처리하였다.

각각 상술한 내용을 적절히 메소드를 활용하여 구현되었으며, 이때 각 값들은 모두 문자열임에 유의하여 작성되었다. 


`int main()`은 다음과 같이 구현되어 있다.
```cpp
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
```
이는 적절히 쿼리함수를 호출하는 것으로 구현되었다.

쿼리함수들은 다음과 같이 구현되어있다.
```cpp
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
```

각각 상술한 내용을 적절히 메소드를 활용하여 구현되었으며, 이때 각 값들은 모두 문자열임에 유의하여 작성되었다. 
