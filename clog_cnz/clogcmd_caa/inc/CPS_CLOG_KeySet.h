#ifndef CPS_CLOG_KeySet_h
#define CPS_CLOG_KeySet_h


#include <iostream>

using namespace std;

/// A single key.
typedef unsigned long long Key_t;

//-----------------------------------------------------------------------------
/// the relation between two sets A and B. When computed by non-static class
/// functions then A is "this" and B is "other".
enum SetRelation
{                                   // A-B B-A T&O&B
   A_AND_B_EMPTY,      ///< dito.   // 0   0   0    = 0
   A_AND_B_EQUAL,      ///< dito.   // 0   0   1    = 1
   A_EMPTY,            ///< dito.   // 0   1   0    = 2
   B_CONTAINS_A,       ///< dito.   // 0   1   1    = 3
   B_EMPTY,            ///< dito.   // 1   0   0    = 4
   A_CONTAINS_B,       ///< dito.   // 1   0   1    = 5
   A_AND_B_DISJOINT,   ///< dito.   // 1   1   0    = 6
   A_AND_B_OVERLAP,    ///< dito.   // 1   1   1    = 7
};

/// how to print keysets and key ranges
enum PrintKeyFormat
{
   KPF_AWKWARD,   ///< e.g. 5&&10&15&&20
   KPF_PRETTY,    ///< e.g. [5-10]+[15-20]
};
//-----------------------------------------------------------------------------
class KeySet;

/// he max. value of a key.
#define MAX_KEY_VALUE 18000000000000000000ULL

/// A single Key or a Key range. A single key K is the range [K-K]
class KeyRange
{
public:
   /// construct an empty KeyRange
   KeyRange()
   : start(0xFFFFFFFFFFFFFFFFULL),
     end(0)
   {}

   /// construct a KeyRange from string \b s (in KPF_AWKWARD format)
   KeyRange(const char * s);

   /// return \b true iff \b this range is empty
   bool is_empty() const   { return (start > end); }

   /// return \b true iff start of \b this is larger than start of other,
   /// or starts are equal and \b other ends before \b this
   bool operator >(const KeyRange & other) const;

   /// print \b this KeyRange on \b out in format \b fmt
   void print(ostream & out, PrintKeyFormat fmt) const;

   /// output length when printed with format \b fmt
   int print_len(PrintKeyFormat fmt) const;

   /// output length of \b key when printed.
   static int print_len(Key_t key);

   /// store intersection with KeyRange \b other into \b result
   /// return 0 if result is empty, else 1.
   int intersection(KeyRange & result, const KeyRange & other) const;

   /// the first element of the key range
   Key_t start;

   /// the last element of the key range
   Key_t end;
};
//-----------------------------------------------------------------------------
/// A set of keys
class KeySet
{
public:
   /// construct a KeySet from string s (in KPF_AWKWARD format)
   KeySet(const char * s);

   /// construct an empty KeySet
   KeySet()
   : item_count(0),
     items(0)
   {}

   /// construct a Keyset with one range from \b start to (including) \b end
   KeySet(Key_t start, Key_t end);

   /// destructor
   ~KeySet();

   /// return the number of ranges in \b this set
   int get_count() const   { return item_count; }

  /// how \b this set \b (A) relates to set \b B
   SetRelation relation(const KeySet * B) const;

   /// return the \b idx'th item in the set
   const KeyRange & operator[](int idx) const;

   /// intersect this KeySet with KeySet \b other
   void intersect_with(const KeySet * other);

   /// store intersection with set \b other into \b result
   void intersection(KeySet & result, const KeySet & other) const;

   /// store \b this without \b other into  \b result
   void diff(KeySet & result, const KeySet & other) const;

   /// return the complement of this set
   void complement(KeySet & result) const;

   /// return true if \b this Keyset is normalized
   bool is_normalized() const;

   /// print \b this KeySet on \b out with format \b fmt
   void print(ostream & out, PrintKeyFormat fmt) const;

   /// output length when printed with format \b fmt
   int print_len(PrintKeyFormat fmt) const;

protected:
   /// order elements and minimize item_count
   void normalize();

   /// return the number of items (ranges or single keys) in \b this KeySet
   static int get_item_count(const char * s);

   /// the number of ranges in \b this set
   int item_count;

   /// the ranges in \b this set
   KeyRange * items;
};
//-----------------------------------------------------------------------------
/*@}*/
#endif // KeySet_h

