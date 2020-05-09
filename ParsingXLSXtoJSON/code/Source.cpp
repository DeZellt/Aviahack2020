#include<iostream>
#include<libxl.h>
#include<string>
#include<rapidjson.h>
#include<document.h>
#include<writer.h>
#include<ostreamwrapper.h>
#include<fstream>

const char* TABLE_NAME = "table.xlsx";
const char* OUTPUT_FILE_FIRST = "table1.json";
const char* OUTPUT_FILE_SECOND = "table2.json";
const char* OUTPUT_FILE_THIRD = "table3.json";
const char* OUTPUT_FILE_FOURTH = "table4.json";
const char* OUTPUT_FILE_FIFTH = "table5.json";

bool ReadFirstSheet(const char* str) {
	libxl::Book* book = xlCreateXMLBook();
	if (!book->load(str)) {
		std::cout << "Book is not loaded\n";
		std::cout << book->errorMessage() << std::endl;
		return false;
	}
	rapidjson::Document json_sheet_table;
	json_sheet_table.SetArray();

	libxl::Sheet* sheet = book->getSheet(0);
	std::string hangar_name;
	double width, depth;
	int i = 1;
	while (true) {
		rapidjson::Document json_sheet_table_cell;
		json_sheet_table_cell.SetObject();

		const char* str = sheet->readStr(i, 0);
		if (!str) {
			break;
		}
		hangar_name = std::string(str);
		rapidjson::Value v;
		v.SetString(hangar_name.c_str(), json_sheet_table.GetAllocator());
		json_sheet_table_cell.AddMember("name", v, json_sheet_table.GetAllocator());

		width = sheet->readNum(i, 1);
		v.SetDouble(width);
		json_sheet_table_cell.AddMember("width", v, json_sheet_table.GetAllocator());

		depth = sheet->readNum(i, 2);
		v.SetDouble(depth);
		json_sheet_table_cell.AddMember("depth", v, json_sheet_table.GetAllocator());
		json_sheet_table.PushBack(json_sheet_table_cell, json_sheet_table.GetAllocator());
		++i;
	}
	std::ofstream out(OUTPUT_FILE_FIRST);
	rapidjson::OStreamWrapper osw(out);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	json_sheet_table.Accept(writer);
	return true;
}

bool ReadSecondSheet(const char* str) {
	libxl::Book* book = xlCreateXMLBook();
	if (!book->load(str)) {
		std::cout << "Book is not loaded\n";
		std::cout << book->errorMessage() << std::endl;
		return false;
	}
	libxl::Sheet* sheet = book->getSheet(1);
	std::string company, start, end, k;
	int i = 1;
	rapidjson::Document json_table;
	json_table.SetArray();
	int d, m, y;
	double s;
	while (true) {
		rapidjson::Document json_table_cell;
		json_table_cell.SetObject();
		const char* str = sheet->readStr(i, 0);
		if (!str) {
			break;
		}
		rapidjson::Value v;
		v.SetString(str, json_table.GetAllocator());
		json_table_cell.AddMember("company", v, json_table.GetAllocator());
		s = sheet->readNum(i, 1);
		book->dateUnpack(s, &y, &m, &d);
		std::string date = std::to_string(d) + '.' + std::to_string(m) + '.' + std::to_string(y);
		v.SetString(date.c_str(), json_table.GetAllocator());
		json_table_cell.AddMember("startTO", v, json_table.GetAllocator());
		s = sheet->readNum(i, 2);
		book->dateUnpack(s, &y, &m, &d);
		date = std::to_string(d) + '.' + std::to_string(m) + '.' + std::to_string(y);
		v.SetString(date.c_str(), json_table.GetAllocator());
		json_table_cell.AddMember("endTO", v, json_table.GetAllocator());
		str = sheet->readStr(i, 3);
		v.SetString(str, json_table.GetAllocator());
		json_table_cell.AddMember("factor", v, json_table.GetAllocator());
		

		json_table.PushBack(json_table_cell, json_table.GetAllocator());
		++i;
	}
	std::ofstream out(OUTPUT_FILE_SECOND);
	rapidjson::OStreamWrapper osw(out);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	json_table.Accept(writer);
	return true;
}

bool ReadThirdSheet(const char* str) {
	libxl::Book* book = xlCreateXMLBook();
	if (!book->load(str)) {
		std::cout << "Book is not loaded\n";
		std::cout << book->errorMessage() << std::endl;
		return false;
	}
	libxl::Sheet* sheet = book->getSheet(2);
	rapidjson::Document json_table;
	json_table.SetArray();
	double length, width;
	int i = 1;
	while (true) {
		rapidjson::Document json_table_cell;
		json_table_cell.SetObject();
		const char* str = sheet->readStr(i, 0);
		if (!str) {
			break;
		}
		rapidjson::Value v;
		v.SetString(str, json_table.GetAllocator());
		json_table_cell.AddMember("name", v, json_table.GetAllocator());
		length = sheet->readNum(i, 1);
		v.SetDouble(length);
		json_table_cell.AddMember("length", v, json_table.GetAllocator());
		width = sheet->readNum(i, 2);
		v.SetDouble(width);
		json_table_cell.AddMember("width", v, json_table.GetAllocator());

		json_table.PushBack(json_table_cell, json_table.GetAllocator());
		++i;
	}
	std::ofstream out(OUTPUT_FILE_THIRD);
	rapidjson::OStreamWrapper osw(out);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	json_table.Accept(writer);
	return true;
}

bool ReadFourthSheet(const char* str) {
	libxl::Book* book = xlCreateXMLBook();
	if (!book->load(str)) {
		std::cout << "Book is not loaded\n";
		std::cout << book->errorMessage() << std::endl;
		return false;
	}
	libxl::Sheet* sheet = book->getSheet(3);
	rapidjson::Document json_table;
	json_table.SetArray();
	int i = 1;
	while (true) {
		rapidjson::Document json_table_cell;
		json_table_cell.SetObject();
		const char* str = sheet->readStr(i, 0);
		if (!str) {
			break;
		}
		rapidjson::Value v;
		v.SetString(str, json_table.GetAllocator());
		json_table_cell.AddMember("TO", v, json_table.GetAllocator());
		const char* name = sheet->readStr(i, 1);
		v.SetString(name, json_table.GetAllocator());
		json_table_cell.AddMember("name", v, json_table.GetAllocator());
		const char* f = sheet->readStr(i, 2);
		if (!f) {
			rapidjson::Value e;
			json_table_cell.AddMember("DME", e, json_table.GetAllocator());
		}
		else {
			v.SetDouble(std::stod(f));
			json_table_cell.AddMember("DME", v, json_table.GetAllocator());
		}
		const char* s = sheet->readStr(i, 3);
		if (!s) {
			rapidjson::Value e;
			json_table_cell.AddMember("SVO", e, json_table.GetAllocator());
		}
		else {
			v.SetDouble(std::stod(s));
			json_table_cell.AddMember("SVO", v, json_table.GetAllocator());
		}
		const char* t = sheet->readStr(i, 4);
		if (!t) {
			rapidjson::Value e;
			json_table_cell.AddMember("VKO", e, json_table.GetAllocator());
		}
		else {
			v.SetDouble(std::stod(t));
			json_table_cell.AddMember("VKO", v, json_table.GetAllocator());
		}
		json_table.PushBack(json_table_cell, json_table.GetAllocator());
		++i;
	}
	std::ofstream out(OUTPUT_FILE_FOURTH);
	rapidjson::OStreamWrapper osw(out);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	json_table.Accept(writer);
	return true;
}

bool ReadFifthSheet(const char* str, int start_point) {
	libxl::Book* book = xlCreateXMLBook();
	if (!book->load(str)) {
		std::cout << "Book is not loaded\n";
		std::cout << book->errorMessage() << std::endl;
		return false;
	}
	book->loadSheet(TABLE_NAME, 4);
	libxl::Sheet* sheet = book->getSheet(4);
	rapidjson::Document json_table;
	json_table.SetArray();
	int i = start_point;
	while (true) {
		rapidjson::Document json_table_cell;
		json_table_cell.SetObject();
		const char* str = sheet->readStr(i, 0);
		if (!str) {
			std::cout << i;
			break;
		}
		rapidjson::Value v;
		v.SetString(str, json_table.GetAllocator());
		json_table_cell.AddMember("company", v, json_table.GetAllocator());
		std::string type(sheet->readStr(i, 1));
		v.SetString(type.c_str(), json_table.GetAllocator());
		json_table_cell.AddMember("type", v, json_table.GetAllocator());
		type = std::string(sheet->readStr(i, 2));
		v.SetString(type.c_str(), json_table.GetAllocator());
		json_table_cell.AddMember("TO", v, json_table.GetAllocator());
		double needs = sheet->readNum(i, 3);
		v.SetDouble(needs);
		json_table_cell.AddMember("needs", v, json_table.GetAllocator());
		double length = sheet->readNum(i, 4);
		v.SetDouble(length);
		json_table_cell.AddMember("length", v, json_table.GetAllocator());
		const char* f = sheet->readStr(i, 5);
		if (!f) {
			rapidjson::Value e;
			json_table_cell.AddMember("min", e, json_table.GetAllocator());
		}
		else {
			v.SetDouble(std::stod(f));
			json_table_cell.AddMember("min", v, json_table.GetAllocator());
		}
		json_table.PushBack(json_table_cell, json_table.GetAllocator());
		++i;
	}
	std::ofstream out("table5_2.json");
	rapidjson::OStreamWrapper osw(out);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	json_table.Accept(writer);
	book->errorMessage();
	return true;
}

int main() {
	//ReadFirstSheet(TABLE_NAME);
	//ReadSecondSheet(TABLE_NAME);
	//ReadThirdSheet(TABLE_NAME);
	//ReadFourthSheet(TABLE_NAME);
	ReadFifthSheet(TABLE_NAME, 50);
	return 0;
}
