
#include <iostream>
#include <iomanip>

#include <mysqlx/xdevapi.h>

int main() {

    try {
        mysqlx::Session session("localhost", 33060, "root", "qwerty");

        mysqlx::Schema schema = session.getSchema("world");
        mysqlx::Table table = schema.getTable("country");

        mysqlx::RowResult result =
                table.select("Code", "Name", "Continent", "Population", "Region")
                        .where("Continent = 'Asia'")
                        .orderBy("Population DESC")
                        .limit(12).execute();

        std::cout << "Query Result:\n";
        for (const auto &item: result.getColumns()) {
            std::cout << std::setw(12) << item.getColumnName() << ",\t";
        }
        std::cout << std::endl;
        while (auto row = result.fetchOne()) {
            for (int i = 0; i < row.colCount(); ++i) {
                std::cout << std::setw(12) << row[i] << ",\t";
            }
            std::cout << std::endl;
        }

        session.close();
    } catch (const mysqlx::Error &err) {
        std::cerr << "Error: " << err << std::endl;
        return 1;
    }


    return 0;
}
