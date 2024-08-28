#include "CPS_CLOG_KeySet.h"
#include "CPS_CLOG_CommandImp.h"
#include <assert.h>
#include <iostream>
#include <string.h>

using namespace std;

//-----------------------------------------------------------------------------
KeySet::KeySet(Key_t start, Key_t end)
   : item_count(1)
{
   items = new KeyRange;
   items[0].start = start;
   items[0].end = end;
}
//-----------------------------------------------------------------------------
KeySet::KeySet(const char * s)
   : item_count(get_item_count(s))
{
   if (item_count == 0)   return;

   items = new KeyRange[item_count];

   for (int item = 0; item < item_count; ++item)
   {
        assert(s);
        items[item] = KeyRange(s);

        const char * amp = strchr(s, '&');
        if (amp && amp[1] == '&')   amp = strchr(amp + 2, '&');   // double '&'

        s = amp + 1;
   }

   normalize();
};
//-----------------------------------------------------------------------------
KeySet::~KeySet()
{
   delete items;
}
//-----------------------------------------------------------------------------
SetRelation
KeySet::relation(const KeySet * k_B) const
{
    KeySet k_common;    this->intersection(k_common, *k_B);
    KeySet k_A_only;    this->diff(k_A_only, *k_B);
    KeySet k_B_only;    k_B->diff(k_B_only, *this);

    int rela = 0;
    if (k_common.get_count())
	   rela |= 1;
    if (k_B_only.get_count()) 
           rela |= 2;
    if (k_A_only.get_count())
           rela |= 4;

    return SetRelation(rela);
}
//-----------------------------------------------------------------------------
const KeyRange &
KeySet::operator[](int idx) const
{
   assert(idx >= 0);
   assert(idx < item_count);
   assert(items);
   return items[idx];
}
//-----------------------------------------------------------------------------
int
KeySet::get_item_count(const char * s)
{
   int single_amps = 0;

   // count the number of single '&'.

   for (const char * ss = s;;)
   {
        const char * amp = strchr(ss, '&');
        if (amp && amp[1] == '&')
	   amp = strchr(amp + 2, '&');   // double '&'

        if (amp == 0)  
	   break;   // no more single &.
        ++single_amps;
        ss = amp + 1;
   }

   if (single_amps > 0)   return single_amps + 1;

   // no single & in key. We have either a single item or none.
   //
   while (s && *s <= ' ')   ++s;
   if (*s)   return 1;
   return 0;
}
//-----------------------------------------------------------------------------
/** sort Key items (by first element) and join adjacent elements and ranges. */
void
KeySet::normalize()
{
   // sort items by start. item_count is supposed to be small, so we
   // can use a simple n^2 algorithm.
   //
   for (int pivot = 0; pivot < item_count; ++pivot)
   {
         int smallest = pivot;
         for (int other = pivot + 1; other < item_count; ++other)
         {
               if (items[smallest] > items[other]) 
		  smallest = other;
         }

         if (smallest != pivot)
         {
              KeyRange temp = items[pivot];
              items[pivot] = items[smallest];
              items[smallest] = temp;
         }
   }

   // now that items are sorted, we can combine adjacent items.
   // If we can combine two adjacent items, then we store the
   // combined result in the lower item and move the items above
   // the pivot item down, descreasing item_count.
   //
   for (int pivot = 1; pivot < item_count; )
   {
         bool combined = false;
         const Key_t n2 = items[pivot]    .start;

         if (n2 <= (items[pivot - 1].end + 1))
         {
              // n2 starts within n1...e1. We end at max(e1, e2).
              //
              if (items[pivot].end > items[pivot - 1].end)
                 items[pivot - 1].end = items[pivot].end;
              combined = true;
         }

         // we have combined items[pivot - 1] and items[p].
         // move items[p + 1]... one position down.
         //
         if (combined)
         {
              for (int p = pivot + 1; p < item_count; ++p)
              {
                    items[p - 1] = items[p];
              }

              --item_count;
         }
         else
         {
              ++pivot;
         }
    }
}
//-----------------------------------------------------------------------------
void
KeySet::intersect_with(const KeySet * other)
{
   KeySet tmp;
   tmp.item_count = item_count;
   tmp.items = items;

   item_count = 0;
   items = 0;

   tmp.intersection(*this, *other);
}
//-----------------------------------------------------------------------------
void
KeySet::intersection(KeySet & result, const KeySet & other) const
{
   assert(result.items == 0);
   assert(result.item_count == 0);

   // if any of the sets is empty, then the result is empty.
   //
   if (item_count == 0)         return;   // result empty.
   if (other.item_count == 0)   return;   // result empty.

   // compute item count
   //
   int count = 0;
   for (int t = 0; t < item_count; ++t)
   {
         const KeyRange & r_t = items[t];
         for (int o = 0; o < other.item_count; ++o)
         {
               const KeyRange & r_o = other.items[o];
               KeyRange tmp;
               count += r_t.intersection(tmp, r_o);
         }
    }

   if (count == 0)   return;

   result.items = new KeyRange[count];

   for (int t = 0; t < item_count; ++t)
   {
         const KeyRange & r_t = items[t];
         for (int o = 0; o < other.item_count; ++o)
         {
               const KeyRange & r_o = other.items[o];
               KeyRange & r_r = result.items[result.item_count];
               const int count =  r_t.intersection(r_r, r_o);
               result.item_count += count;
         }
   }

   if (log_control & LOG_KEY_DEBUG_2)
   {
        cerr << "INTERSECTION of ";
        print(cerr, KPF_PRETTY);
        cerr << " and ";
        other.print(cerr, KPF_PRETTY);
        cerr << " is ";
        result.print(cerr, KPF_PRETTY); cerr << endl;
   }
}
//-----------------------------------------------------------------------------
void
KeySet::complement(KeySet & result) const
{
   assert(result.items == 0);
   assert(result.item_count == 0);

   if (item_count == 0)   // this is empty
   {
        result.item_count = 1;
        result.items = new KeyRange;
        result.items[0].start = 0;
        result.items[0].end = MAX_KEY_VALUE;
        return;
   }

   if (!is_normalized())
   {
        cerr << "UNNORMALIZED(" << item_count << ") : ";
        print(cerr, KPF_PRETTY);
        cerr << endl;
        assert(0 && "KeySet not normalized");
   }

   // the complement has (item_count - 1) ranges between our ranges, and
   // maybe a range c0 before and maybe a range cN after our ranges.
   //
   // this:            [r0]    [r1]     ... [rN-1]
   // complement:  [c0]    [c1]    [c2] ...       [cN]
   //
   int count = item_count - 1;                                   // c1...
   if (items[0].start != 0)                          ++count; // c0
   if (items[item_count - 1].end != MAX_KEY_VALUE)   ++count; // cN

   if (count == 0)   return;

   result.items = new KeyRange[count];
   result.item_count = 0;

   if (items[0].start != 0)   // cS
   {
        KeyRange & r = result.items[result.item_count++];
        r.start = 0;
        r.end   = items[0].start - 1;
   }

   for (int i = 1; i < item_count; ++i)   // c1 ...
   {
        KeyRange & r = result.items[result.item_count++];
        r.start = items[i - 1].end + 1;
        r.end   = items[i].start   - 1;
   }

   if (items[item_count - 1].end != MAX_KEY_VALUE) // cN
   {
        KeyRange & r = result.items[result.item_count++];
        r.start = items[item_count - 1].end + 1;
        r.end   = MAX_KEY_VALUE;
   }

   if (log_control & LOG_KEY_DEBUG_2)
   {
        cerr << "COMPLEMENT of ";
        print(cerr, KPF_PRETTY);
        cerr << " is ";
        result.print(cerr, KPF_PRETTY);
        cerr << endl;
   }
}
//-----------------------------------------------------------------------------
bool
KeySet::is_normalized() const
{
   if (item_count == 0)   return true;

   if (items[0].start > items[0].end)   return false;

   for (int i = 1; i < item_count; ++i)
   {
         if (items[i].start > items[i]    .end)   return false;
         if (items[i].start < items[i - 1].end)   return false;
   }

   return true;
}
//-----------------------------------------------------------------------------
void
KeySet::diff(KeySet & result, const KeySet & other) const
{
   KeySet comp;
   other.complement(comp);

   this->intersection(result, comp);

   if (log_control & LOG_KEY_DEBUG_2)
   {
        cerr << "DIFFERENCE ";
        print(cerr, KPF_PRETTY);
        cerr << " without ";
        other .print(cerr, KPF_PRETTY);
        cerr << " is ";
        result.print(cerr, KPF_PRETTY); cerr << endl;
   }
}
//-----------------------------------------------------------------------------
void
KeySet::print(ostream & out, PrintKeyFormat fmt) const
{
   if (item_count == 0)   // empty
   {
        if (fmt == KPF_PRETTY)   out << "empty";
        return;
   }

   items[0].print(out, fmt);
   for (int i = 1; i < item_count; ++i)
   {
         if (fmt == KPF_PRETTY)   out << "+";
         else                     out << "&";
         items[i].print(out, fmt);
   }
}
//-----------------------------------------------------------------------------
int
KeySet::print_len(PrintKeyFormat fmt) const
{
   if (item_count == 0)   // empty
   {
        if (fmt == KPF_PRETTY)   return 5;
        else                     return 0;
   }

   int len = items[0].print_len(fmt);

   for (int i = 1; i < item_count; ++i)
   {
         len += 1 + items[i].print_len(fmt);
   }

   return len;
}
//=============================================================================
KeyRange::KeyRange(const char * s)
   : start(0),
     end(0)
{
   if (log_control & LOG_KEY_DEBUG_2)
     cerr << "KeyRange::KeyRange(" << s << ") ";

   while(*s)
   {
        char cc = *s++;
        if (cc <= ' ')   continue;
        if (cc == '&')          // end or second element of range,
        {
             if (s[0] == '&')   // second element of range (e.g. 2&&5).
             {
                  ++s;
                  while(*s)
                  {
                        cc = *s++;
                        if (cc <= ' ')               continue;
                        if (cc < '0' || cc > '9')   break;

                        end *= 10;
                        end += cc - '0';
                  }

                  if (log_control & LOG_KEY_DEBUG_2)
                    cerr << start << " to " << end << " (range) " << endl;
                  return;
             }
             else               // not &&
             {
                 break;
             }
         }

        if (cc < '0' || cc > '9')   break;

        start *= 10;
        start += cc - '0';
    }

   end = start;

  if (log_control & LOG_KEY_DEBUG_2)
      cerr << start << " to " << end << " (single) " << endl;
}
//-----------------------------------------------------------------------------
bool
KeyRange::operator >(const KeyRange & other) const
{
   if (start == other.start)   return (end > other.end);
   return (start > other.start);
}
//-----------------------------------------------------------------------------
void
KeyRange::print(ostream & out, PrintKeyFormat fmt) const
{
   if (fmt == KPF_PRETTY)
   {
        out << "[";
        if (is_empty())          out << "-empty-";
        else if (start == end)   out << start;
        else                     out << start << "-" << end;
        out << "]";
   }
   else
   {
        if (is_empty())          ;
        else if (start == end)   out << start;
        else                     out << start << "&&" << end;
   }
}
//-----------------------------------------------------------------------------
int
KeyRange::print_len(PrintKeyFormat fmt) const
{
   if (fmt == KPF_PRETTY)
   {
        if (is_empty())     return 2 + 5;
        if (start == end)   return 2 + print_len(start);
        return                     2 + print_len(start) + 1 + print_len(end);
   }
   else
   {
        if (is_empty())     return 5;
        if (start == end)   return print_len(start);
        return                     print_len(start) + 2 + print_len(end);
   }
}
//-----------------------------------------------------------------------------
int
KeyRange::print_len(Key_t key)
{
   int len = 0;
   for (; key; ++len)   key /= 10;
   return len ? len : 1;
}
//-----------------------------------------------------------------------------
int
KeyRange::intersection(KeyRange & result, const KeyRange & other) const
{
   if (is_empty())          return 0;
   if (other.is_empty())    return 0;

   assert(start <= end);
   assert(other.start <= other.end);

   Key_t s = start;
   Key_t e = end;
   int ret;

   if (s < other.start)   s = other.start;
   if (e > other.end)     e = other.end;

   if (s <= e)   // non-empty result
   {
        ret = 1;
        result.start = s;
        result.end = e;
   }
   else          // empty result
   {
        ret = 0;
   }

   if (0 && log_control & LOG_KEY_DEBUG_2)
   {
         cerr << "intersection ";
         print(cerr, KPF_PRETTY);
         cerr << " and ";
         other.print(cerr, KPF_PRETTY);
         cerr << " is ";
         if (ret)   result.print(cerr, KPF_PRETTY);
         else       cerr << "[]";
         cerr << endl;
   }

   return ret;
}
//=============================================================================
/*@}*/


