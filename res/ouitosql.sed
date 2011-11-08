#n
# Above line = no auto print for sed

# CREATE the tables if they don't exists
1{i \
CREATE TEMPORARY TABLE prefix_temp(mac TEXT UNIQUE, vendor INTEGER REFERENCES vendor(id) ON UPDATE CASCADE ON DELETE RESTRICT);\
CREATE TEMPORARY TABLE vendor_temp(id INTEGER PRIMARY KEY, name TEXT UNIQUE);
}

# Quote the quotes for SQL commands
# SQLite wants the quotes to be _doubled_ and not escaped with a backslash
s/"/""/g

# Match only lines with mac and vendor
/hex/{
#replace the mac and vendor with sql statements
s/\(..-..-..\).*(hex)[ \t]*\(.*\)$/\
INSERT OR IGNORE INTO vendor_temp(name) VALUES ("\2");\
INSERT OR IGNORE INTO prefix_temp(mac,vendor) SELECT "\1",id FROM vendor_temp WHERE name LIKE "\2";/p
}

# CREATE the final tables using the temporary ones
${a \
CREATE TABLE prefix AS SELECT * FROM prefix_temp;\
CREATE TABLE vendor AS SELECT * FROM vendor_temp;
}
