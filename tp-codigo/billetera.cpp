#include <vector>

#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

Billetera::Billetera(const id_billetera id, Blockchain* blockchain)
  : _id(id)
  , _blockchain(blockchain){
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
    _transacciones_por_billetera[t.destino]++; // 
  } 
  
  timestamp fin_de_dia = Calendario::fin_del_dia(t._timestamp); // 0(1)
  _saldos_diarios[fin_de_dia] = _saldo; // 

}

monto Billetera::saldo() const {
  return _saldo; // O(1)
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  // const list<Transaccion> transacciones = _blockchain->transacciones();
  timestamp fin_del_dia = Calendario::fin_del_dia(t);

  // monto ret = 0;

  // auto it = transacciones.begin();
  // while (it != transacciones.end() && it->_timestamp < fin_del_dia) {
  //   if (it->origen == _id) {
  //     ret -= it->monto;
  //   } else if (it->destino == _id) {
  //     ret += it->monto;
  //   }
  //   ++it;
  // }

  // return ret;

  return _saldos_diarios.at(fin_del_dia);
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const {

  vector<Transaccion> ret;

  auto it = _relevantes.rbegin();
  while(it != _relevantes.rend() && ret.size() < k) {
      ret.push_back(*it);
    ++it;
  }

  return ret;
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {

   vector<id_billetera> ret;
    for (auto it = _transacciones_por_billetera.rbegin(); it != _transacciones_por_billetera.rend() && ret.size() < k; ++it) {
      ret.push_back(_transacciones_por_billetera.at(1));
      it++;
    }

    return ret;

  // const list<Transaccion> transacciones = _blockchain->transacciones();

  // // cuento la cantidad de transacciones salientes por cada billetera de destino
  // map<id_billetera, int> transacciones_por_billetera;
  // auto it = transacciones.begin();
  // while (it != transacciones.end()) {
  //   if (it->origen == _id) {
  //     // notar que el map devuelve 0 por default!
  //     transacciones_por_billetera[it->destino]++;
  //   }
  //   ++it;
  // }

  // // invierto el map de forma que puedo accedes a las billeteras según su
  // // cantidad de transacciones.
  // map<int, vector<id_billetera>> billeteras_por_cantidad_de_transacciones;
  // auto it2 = transacciones_por_billetera.begin();
  // while (it2 != transacciones_por_billetera.end()) {
  //   billeteras_por_cantidad_de_transacciones[it2->second].push_back(it2->first);
  //   ++it2;
  // }

  // // recorro el map usando un iterador en orden inverso, que me lleva por todos
  // // los pares de entradas desde la mayor clave hasta la menor.
  // vector<id_billetera> ret = {};
  // auto it3 = billeteras_por_cantidad_de_transacciones.rbegin();
  // while (it3 != billeteras_por_cantidad_de_transacciones.rend() && ret.size() < k) {
  //   vector<id_billetera> siguiente_grupo = it3->second;
  //   int i = 0;
  //   while (i < siguiente_grupo.size() && ret.size() < k) {
  //     ret.push_back(siguiente_grupo[i]);
  //     i++;
  //   }
  //   ++it3;
  // }

  // return ret;
}
