/*****************************************************************************
*
* Copyright (c) 2000 - 2013, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

#ifndef DBOPTIONSATTRIBUTES_H
#define DBOPTIONSATTRIBUTES_H
#include <state_exports.h>
#include <string>
#include <AttributeSubject.h>


// ****************************************************************************
// Class: DBOptionsAttributes
//
// Purpose:
//    Attributes of database options
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

class STATE_API DBOptionsAttributes : public AttributeSubject
{
public:
    enum OptionType
    {
        Bool,
        Int,
        Float,
        Double,
        String,
        Enum
    };

    // These constructors are for objects of this class
    DBOptionsAttributes();
    DBOptionsAttributes(const DBOptionsAttributes &obj);
protected:
    // These constructors are for objects derived from this class
    DBOptionsAttributes(private_tmfs_t tmfs);
    DBOptionsAttributes(const DBOptionsAttributes &obj, private_tmfs_t tmfs);
public:
    virtual ~DBOptionsAttributes();

    virtual DBOptionsAttributes& operator = (const DBOptionsAttributes &obj);
    virtual bool operator == (const DBOptionsAttributes &obj) const;
    virtual bool operator != (const DBOptionsAttributes &obj) const;
private:
    void Init();
    void Copy(const DBOptionsAttributes &obj);
public:

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();
    void SelectTypes();
    void SelectNames();
    void SelectOptBools();
    void SelectOptFloats();
    void SelectOptDoubles();
    void SelectOptInts();
    void SelectOptStrings();
    void SelectOptEnums();
    void SelectEnumStrings();
    void SelectEnumStringsSizes();
    void SelectObsoleteNames();

    // Property setting methods
    void SetTypes(const intVector &types_);
    void SetNames(const stringVector &names_);
    void SetOptBools(const intVector &optBools_);
    void SetOptFloats(const doubleVector &optFloats_);
    void SetOptDoubles(const doubleVector &optDoubles_);
    void SetOptInts(const intVector &optInts_);
    void SetOptStrings(const stringVector &optStrings_);
    void SetOptEnums(const intVector &optEnums_);
    void SetEnumStrings(const stringVector &enumStrings_);
    void SetEnumStringsSizes(const intVector &enumStringsSizes_);
    void SetObsoleteNames(const stringVector &obsoleteNames_);

    // Property getting methods
    const intVector    &GetTypes() const;
          intVector    &GetTypes();
    const stringVector &GetNames() const;
          stringVector &GetNames();
    const intVector    &GetOptBools() const;
          intVector    &GetOptBools();
    const doubleVector &GetOptFloats() const;
          doubleVector &GetOptFloats();
    const doubleVector &GetOptDoubles() const;
          doubleVector &GetOptDoubles();
    const intVector    &GetOptInts() const;
          intVector    &GetOptInts();
    const stringVector &GetOptStrings() const;
          stringVector &GetOptStrings();
    const intVector    &GetOptEnums() const;
          intVector    &GetOptEnums();
    const stringVector &GetEnumStrings() const;
          stringVector &GetEnumStrings();
    const intVector    &GetEnumStringsSizes() const;
          intVector    &GetEnumStringsSizes();
    const stringVector &GetObsoleteNames() const;
          stringVector &GetObsoleteNames();

    // Persistence methods
    virtual bool CreateNode(DataNode *node, bool completeSave, bool forceAdd);
    virtual void SetFromNode(DataNode *node);

    // Enum conversion functions
    static std::string OptionType_ToString(OptionType);
    static bool OptionType_FromString(const std::string &, OptionType &);
protected:
    static std::string OptionType_ToString(int);
public:

    // Keyframing methods
    virtual std::string               GetFieldName(int index) const;
    virtual AttributeGroup::FieldType GetFieldType(int index) const;
    virtual std::string               GetFieldTypeName(int index) const;
    virtual bool                      FieldsEqual(int index, const AttributeGroup *rhs) const;

    // User-defined methods
    int FindIndex(const std::string &name) const;
    void SetBool(const std::string &name, bool defaultValue);
    bool GetBool(const std::string &name) const;
    void SetFloat(const std::string &name, float defaultValue);
    float GetFloat(const std::string &name) const;
    void SetDouble(const std::string &name, double defaultValue);
    double GetDouble(const std::string &name) const;
    void SetInt(const std::string &name, int defaultValue);
    int GetInt(const std::string &name) const;
    void SetString(const std::string &name, const std::string &defaultValue);
    const std::string &GetString(const std::string &name) const;
    void SetEnum(const std::string &name, int defaultValue);
    int GetEnum(const std::string &name) const;
    void SetEnumStrings(const std::string &name, const std::vector<std::string> &values);
    int GetNumberOfOptions(void) const;
    DBOptionsAttributes::OptionType GetType(int) const;
    std::vector<std::string> GetEnumStrings(const std::string &name) const;
    std::string GetName(int) const;
    void SetObsolete(const std::string &name);
    bool IsObsolete(const std::string &name) const;

    // IDs that can be used to identify fields in case statements
    enum {
        ID_types = 0,
        ID_names,
        ID_optBools,
        ID_optFloats,
        ID_optDoubles,
        ID_optInts,
        ID_optStrings,
        ID_optEnums,
        ID_enumStrings,
        ID_enumStringsSizes,
        ID_obsoleteNames,
        ID__LAST
    };

private:
    intVector    types;
    stringVector names;
    intVector    optBools;
    doubleVector optFloats;
    doubleVector optDoubles;
    intVector    optInts;
    stringVector optStrings;
    intVector    optEnums;
    stringVector enumStrings;
    intVector    enumStringsSizes;
    stringVector obsoleteNames;

    // Static class format string for type map.
    static const char *TypeMapFormatString;
    static const private_tmfs_t TmfsStruct;
};
#define DBOPTIONSATTRIBUTES_TMFS "i*s*i*d*d*i*s*i*s*i*s*"

#endif
