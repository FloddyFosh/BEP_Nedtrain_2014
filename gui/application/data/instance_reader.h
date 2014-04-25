#ifndef INSTANCEREADER_H_
#define INSTANCEREADER_H_

#include "model/instance.h"

class InstanceReader {
public:
    /**
     * Load instance definition and working solution from file.
     * @param fileName The filename of the instance.
     * @return A new Instance object.
     */
    static Instance* load(const QString &fileName);

    /**
     * Import instance in the instance loaded previously
     * @param instance Instance to import new instance in
     * @param fileName The filename of the instance
     * @return true on success, false otherwise
     */
    static bool import(Instance *instance, const QString &fileName);

    /**
     * Read an input file and add the data to an instance.
     * @param fileName The filename of the input file.
     * @param instance The instance which the input is added to.
     * @return true on success, false otherwise
     */
    static bool readIntoInstance(const QString &fileName, Instance *instance);

    /**
     * Saves instance definition and working solution to file.
     * @param instance The instance that is to be saved.
     * @param fileName
     * @return true on success, false otherwise
     */
    static bool save(Instance *instance, const QString &fileName);

    /**
     * Creates a copy of the instance by reading the string representation.
     * @param instance The instance that is to be copied.
     * @return A copy of the instance.
     */
    static Instance* copy(Instance *instance);
};

#endif /* INSTANCEREADER_H_ */
