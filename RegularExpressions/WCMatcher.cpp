#include <regexp/WCMatcher.h>
#include <regexp/WCPattern.h>

const int WCMatcher::MATCH_ENTIRE_STRING = 0x01;

/*
  Detailed documentation is provided in this class' header file

  @author   Jeffery Stuart
  @since    November 2004
  @version  1.07.00
*/

WCMatcher::WCMatcher(WCPattern * pattern, const std::wstring & text)
{
  pat = pattern;
  str = text;
  gc = pattern->groupCount;
  ncgc = -pattern->nonCapGroupCount;
  flags = 0;
  matchedSomething = false;
  starts        = new int[gc + ncgc];
  ends          = new int[gc + ncgc];
  groups        = new int[gc + ncgc];
  groupPos      = new int[gc + ncgc];
  groupIndeces  = new int[gc + ncgc];
  starts        = starts        + ncgc;
  ends          = ends          + ncgc;
  groups        = groups        + ncgc;
  groupPos      = groupPos      + ncgc;
  groupIndeces  = groupIndeces  + ncgc;
  for (int i = 0; i < gc; ++i) starts[i] = ends[i] = 0;
}
WCMatcher::~WCMatcher()
{
  delete [] (starts       - ncgc);
  delete [] (ends         - ncgc);
  delete [] (groups       - ncgc);
  delete [] (groupIndeces - ncgc);
  delete [] (groupPos     - ncgc);
}
void WCMatcher::clearGroups()
{
  int i;
  lm = 0;
  for (i = 0; i < gc; ++i)    groups[i] = starts[i] = ends[i] = -1;
  for (i = 1; i <= ncgc; ++i) groups[0 - i] = starts[0 - i] = ends[0 - i] = -1;
}
std::wstring WCMatcher::replaceWithGroups(const std::wstring & str)
{
  std::wstring ret = L"";

  std::wstring t = str;
  while (t.size() > 0)
  {
    if (t[0] == (wchar_t)'\\')
    {
      t = t.substr(1);
      if (t.size() == 0)
      {
        ret += L"\\";
      }
      else if (t[0] < (wchar_t)'0' || t[0] > (wchar_t)'9')
      {
        ret += t.substr(0, 1);
        t = t.substr(1);
      }
      else
      {
        int gn = 0;
        while (t.size() > 0 && t[0] >= (wchar_t)'0' && t[0] <= (wchar_t)'9')
        {
          gn = gn * 10 + (t[0] - (wchar_t)'0');
          t = t.substr(1);
        }
        ret += getGroup(gn);
      }
    }
    else
    {
      ret += t.substr(0, 1);
      t = t.substr(1);
    }
  }

  return ret;
}
unsigned long WCMatcher::getFlags() const
{
  return flags;
}
std::wstring WCMatcher::getText() const
{
  return str;
}

bool WCMatcher::matches()
{
  flags = MATCH_ENTIRE_STRING;
  matchedSomething = false;
  clearGroups();
  lm = 0;
  return pat->head->match(str, this, 0) == (int)str.size();
}
bool WCMatcher::findFirstMatch()
{
  starts[0] = 0;
  flags = 0;
  clearGroups();
  start = 0;
  lm = 0;
  ends[0] = pat->head->match(str, this, 0);
  if (ends[0] >= 0)
  {
    matchedSomething = true;
    return 1;
  }
  return 0;
}
bool WCMatcher::findNextMatch()
{
  int s = starts[0], e = ends[0];

  if (!matchedSomething) return findFirstMatch();
  if (s == e) ++e;
  flags = 0;
  clearGroups();

  starts[0] = e;
  if (e >= (int)str.size()) return 0;
  start = e;
  lm = e;
  ends[0] = pat->head->match(str, this, e);
  return ends[0] >= 0;
}
std::vector<std::wstring> WCMatcher::findAll()
{
  std::vector<std::wstring> ret;
  reset();
  while (findNextMatch())
  {
    ret.push_back(getGroup());
  }
  return ret;
}

void WCMatcher::reset()
{
  lm = 0;
  clearGroups();
  matchedSomething = false;
}

int WCMatcher::getStartingIndex(const int groupNum) const
{
  if (groupNum < 0 || groupNum >= gc) return -1;
  return starts[groupNum];
}
int WCMatcher::getEndingIndex(const int groupNum) const
{
  if (groupNum < 0 || groupNum >= gc) return -1;
  return ends[groupNum];
}
std::wstring WCMatcher::getGroup(const int groupNum) const
{
  if (groupNum < 0 || groupNum >= gc) return L"";
  if (starts[groupNum] < 0 || ends[groupNum] < 0) return L"";
  return str.substr(starts[groupNum], ends[groupNum] - starts[groupNum]);
}
std::vector<std::wstring> WCMatcher::getGroups(const bool includeGroupZero) const
{
  int i, start = (includeGroupZero ? 0 : 1);
  std::vector<std::wstring> ret;

  for (i = start; i < gc; ++i)
  {
    ret.push_back(getGroup(i));
  }
  return ret;
}

