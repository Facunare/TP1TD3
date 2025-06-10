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

  bool transaccion_relevante = t.origen == _id || t.destino == _id;
  if (transaccion_relevante) {
    _relevantes.push_back(t);
  }

  const list<Transaccion> transacciones = _blockchain->transacciones();
  auto it = transacciones.begin();

  if(t.destino == _id){
   _saldo += t.monto;
  }else if(t.origen == _id){
   _saldo -= t.monto;
  } 
  
  timestamp fin_de_dia = Calendario::fin_del_dia(t._timestamp);
  _saldos_diarios[fin_de_dia] = _saldo;

  // Agregar las transacciones relevantes a _relevantes. Asi hacemos en orden k ultimas_transacciones
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
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {
  const list<Transaccion> transacciones = _blockchain->transacciones();

  // cuento la cantidad de transacciones salientes por cada billetera de destino
  map<id_billetera, int> transacciones_por_billetera;
  auto it = transacciones.begin();
  while (it != transacciones.end()) {
    if (it->origen == _id) {
      // notar que el map devuelve 0 por default!
      transacciones_por_billetera[it->destino]++;
    }
    ++it;
  }

  // invierto el map de forma que puedo accedes a las billeteras según su
  // cantidad de transacciones.
  map<int, vector<id_billetera>> billeteras_por_cantidad_de_transacciones;
  auto it2 = transacciones_por_billetera.begin();
  while (it2 != transacciones_por_billetera.end()) {
    billeteras_por_cantidad_de_transacciones[it2->second].push_back(it2->first);
    ++it2;
  }

  // recorro el map usando un iterador en orden inverso, que me lleva por todos
  // los pares de entradas desde la mayor clave hasta la menor.
  vector<id_billetera> ret = {};
  auto it3 = billeteras_por_cantidad_de_transacciones.rbegin();
  while (it3 != billeteras_por_cantidad_de_transacciones.rend() && ret.size() < k) {
    vector<id_billetera> siguiente_grupo = it3->second;
    int i = 0;
    while (i < siguiente_grupo.size() && ret.size() < k) {
      ret.push_back(siguiente_grupo[i]);
      i++;
    }
    ++it3;
  }

  return ret;
}
