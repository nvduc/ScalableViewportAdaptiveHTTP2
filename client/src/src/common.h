#include <iostream>
#include <vector>
using namespace std;

double calc_avg_visi_tile_qua(int* tile_ver, vector<int> visi_tile){
  double ret = 0;
  for(int i=0; i < visi_tile.size(); i++)
    ret += tile_ver[visi_tile[i]]*1.0/visi_tile.size();
  return ret;
}
