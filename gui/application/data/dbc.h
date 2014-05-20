#ifndef DBC_H_
#define DBC_H_

#include <QString>

/**
 * This class represents a database connection and facilitates functions to
 * open and close the connection with the SQLite database.
 */
class dbc
{
    QString connectionName;
public:
    /**
     * Constructs a dbc object. Initializes SQLite and database.
     */
	dbc();

	/**
	 * Destructs a dbc object. Closes a database connection and removes the
	 * SQLite database type from the Qt database connector.
	 */
	~dbc();

	/**
	 * Opens a database connection.
	 */
	void open();

	/**
	 * Closes the database connection if connected.
	 */
    void close();

    /**
     * Returns if the database is open.
     */
    bool isOpenDB();
};

#endif /* DBC_H_ */
