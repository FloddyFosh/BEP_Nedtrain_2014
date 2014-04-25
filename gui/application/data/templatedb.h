#ifndef TEMPLATEDB_H_
#define TEMPLATEDB_H_

#include <QSqlQuery>
#include <QVariant>

#include "data/dbc.h"
#include "data/i_template_gateway.h"

/**
 * The template db contains activity templates. This class provides methods to get,
 * modify and remove templates with associated requirements in/from the database.
 */

class TemplateDB : public ITemplateGateway
{
    dbc *db; ///< pointer to the database controller, used to instantiate db connection

    /**
     * Prints error if query execution went wrong.
     * @param query executed query.
     */
    void showQueryError(QSqlQuery query);

    /**
     * Gets the associated requirements of an activity template from the database.
     * @param id the id of the activity template.
     */
    QMap<QString, int> getRequirements(int id);

    /**
     * Saves requirements in the database with the specified template id.
     * @param map the map containing requirements.
     * @param lastId the id of the associated activity template.
     */
    void saveRequirements(QMap<QString, int> map, QVariant lastId);

    /**
     * Remove associated requirements of an activity template from database.
     * @param id id of the activity template.
     */
    void removeRequirements(int id);

public:
    /**
     * Constructor used when instantiating template db.
     */
    TemplateDB();

    /**
     * Destructor used when closing template db, also destructs underlying db connection.
     */
    ~TemplateDB();

    /**
     * Saves activity template in the database.
     * @param activity activity to be saved.
     */
    void save(ActivityTemplate *activity);

    /**
     * Gets activity template with specified id from the database, otherwise null pointer.
     * @param id id of the activity template.
     */
    ActivityTemplate* get(int id);

    /** Gets all activitie templates from the database.
      */
	QList<ActivityTemplate *> getAll();

	/**
	 * Removes activity template with specified id from the database.
	 * @param id id of the activity template to be removed.
	 */
	void remove(int id);

	/**
	 * Removes activity template from the database.
	 * @param activity activity template to be removed, must contain a valid id.
	 */
    void remove(ActivityTemplate *activity);

    /**
     * Updates an existing activity template in the database.
     * @param activity the template to be updated.
     */
    void update(ActivityTemplate *activity);

};

#endif /* TEMPLATEDB_H_ */
