#include <vector>

#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

Billetera::Billetera(const id_billetera id, Blockchain* blockchain)
  : _id(id)
  , _blockchain(blockchain)
  , _saldo(0) {
}

id_billetera Billetera::id() const {
  return _id;
}

void Billetera::notificar_transaccion(Transaccion t) {

    bool transaccion_relevante = t.origen == _id || t.destino == _id; // O(1)
    if (transaccion_relevante) { // O(1)
      _relevantes.push_back(t); // 0(1)
    }

    const list<Transaccion> transacciones = _blockchain->transacciones(); // 0(1)
    auto it = transacciones.begin(); // 0(1)

    if(t.destino == _id){ // 0(1)
    _saldo += t.monto; // 0(1)
    }else if(t.origen == _id){ // O(1)
      _saldo -= t.monto; // 0(1)
      

    bool encontre = false;
    for(int i=_transacciones_por_billetera.size() -1 ; i>=0; i--){ // O(C)
      
      if(_transacciones_por_billetera[i].second == t.destino){ // O(1)
        _transacciones_por_billetera[i].first++; // O(1)
        encontre = true; // O(1)
      }

      if(encontre && i>0 && _transacciones_por_billetera[i].first > _transacciones_por_billetera[i-1].first){ // O(1)
        swap(_transacciones_por_billetera[i], _transacciones_por_billetera[i-1]); // O(1)
      }
  
    }

    if(!encontre){ // O(1)
      _transacciones_por_billetera.push_back({1, t.destino}); 
      int j = _transacciones_por_billetera.size() - 1; // O(1)
      while(j > 0 && _transacciones_por_billetera[j].first > _transacciones_por_billetera[j-1].first){ // O(C)
        swap(_transacciones_por_billetera[j], _transacciones_por_billetera[j-1]); // O(1)
        j--; // O(1)
      }
    }
  } 

  timestamp fin_de_dia = Calendario::fin_del_dia(t._timestamp); // 0(1)
  // _saldos_diarios[fin_de_dia] = _saldo; 

  bool encontrado = false;
  for (int i = _saldos_diarios2.size() - 1; i >= 0; i--) { // O(D)
      if (_saldos_diarios2[i].first == fin_de_dia) { // O(1)
          _saldos_diarios2[i].second = _saldo; // O(1)
          encontrado = true; // O(1)
          break; // O(1)
      }
  }

  if (!encontrado) { // O(1)
      _saldos_diarios2.push_back({fin_de_dia, _saldo}); // O(1)
  }

}

monto Billetera::saldo() const {
  return _saldo; // O(1)
  // O(1)
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  timestamp fin_del_dia = Calendario::fin_del_dia(t); // O(1)

  int izq = 0;
  int der = _saldos_diarios2.size() - 1;

  while(izq<=der){
    int medio = (izq+der)/2;
    if(_saldos_diarios2[medio].first == fin_del_dia){
      return _saldos_diarios2[medio].second;
    }else if(_saldos_diarios2[medio].first < fin_del_dia){
      izq = medio + 1;
    }else{
      der = medio - 1;
    }
  }

  if(der>=0){
    return _saldos_diarios2[der].second;
  }

  return 0;
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const {

  vector<Transaccion> ret; // O(1)

  auto it = _relevantes.rbegin(); // 0(1)
  while(it != _relevantes.rend() && ret.size() < k) { // k iteraciones | Guarda: O(1) 
      ret.push_back(*it); // O(1)
    ++it; // 0(1)
  }

  return ret; // O(1)

  // O(1) + O(1) + O(k)*[O(1)+O(1)] + O(1) + O(1) 
  // O(1) + O(k)*O(1) + O(1)
  // O(1) + O(k)
  // O(k)
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {

  vector<id_billetera> ret = {}; // O(1)
  int i = 0; // O(1)
  while (i<_transacciones_por_billetera.size() && ret.size() < k) { // k iteraciones | Guarda: O(1) 
    ret.push_back(_transacciones_por_billetera[i].second); // O(1)
    i++; // O(1)
  }
  return ret; // O(1)

  // O(1) + O(1) + O(k)*[O(1)+O(1)] + O(1) + O(1) 
  // O(1) + O(k)*O(1) + O(1)
  // O(1) + O(k)
  // O(k)
}
