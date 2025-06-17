#include <vector>

#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

// Rep(e : Billetera):
// - todas las transacciones de e._relevantes cumplen que tienen como origen o destino a esta billetera
// - se actualiza el saldo dependiendo si fue el que emitio o recibio la transaccion.
// - el vector e._transacciones_por_billetera esta ordenado descendentemnente segun las billeteras con mas transacciones
// - la cantidad de transaccion asociado a un id en e._transacciones_por_billetera es mayor o igual a 1
// - no hay id_billetera repetidos en e._transaccion_por_billetera y e._id != e._id_billetera
// - todos los id_billetera pertenecen a e._blockchain.billeteras
// - el vector e,_saldos_diarios esta ordenado ascendentemente segun el timestamp. El segundo valor del par es el saldo final del dia especficado por el primer valor.
// - no hay timestamps repetidos en e._saldos_diarios
// - el monto en e._saldos_diarios es mayor o igual a 0
// - e._saldo es igual a sumar a e._saldo inicial el monto para cada valor de _relevantes cuando esta billetera fue destino y restar cuando fue origen

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
      _relevantes.push_back(t); // 0(1) amortizado
    }

    const list<Transaccion> transacciones = _blockchain->transacciones(); // 0(1)

    if(t.destino == _id){ // 0(1)
    _saldo += t.monto; // 0(1)
    }else if(t.origen == _id){ // O(1)
      _saldo -= t.monto; // 0(1)
    
    bool encontre = false; // O(1)

    // Complejidad total del ciclo: O(C)
    // - O(C) iteraciones
    // - O(1) operaciones en cada iteracion
    for(int i=_transacciones_por_billetera.size() -1 ; i>=0; i--){ // O(C) iteraciones | Guarda: O(1) 
      
      if(_transacciones_por_billetera[i].second == t.destino){ // O(1)
        _transacciones_por_billetera[i].first++; // O(1)
        encontre = true; // O(1)
      }

      if(encontre && i>0 && _transacciones_por_billetera[i].first > _transacciones_por_billetera[i-1].first){ // O(1)
        swap(_transacciones_por_billetera[i], _transacciones_por_billetera[i-1]); // O(1)
      }

    }

    if(!encontre){ // O(1)
      _transacciones_por_billetera.push_back({1, t.destino});  // O(1) amortizado 
      int j = _transacciones_por_billetera.size() - 1; // O(1)
      // Complejidad total del ciclo: O(C)
      // - O(C) iteraciones
      // - O(1) operaciones en cada iteracion
      while(j > 0 && _transacciones_por_billetera[j].first > _transacciones_por_billetera[j-1].first){ // O(C) iteraciones | Guarda: O(1) 
        swap(_transacciones_por_billetera[j], _transacciones_por_billetera[j-1]); // O(1)
        j--; // O(1)
      }
    }
  }
     

  timestamp fin_de_dia = Calendario::fin_del_dia(t._timestamp); // 0(1)

  bool encontrado = false; // O(1)
  // Complejidad total del ciclo: O(D)
  // - O(D) iteraciones
  // - O(1) operaciones en cada iteracion
  for (int i = _saldos_diarios.size() - 1; i >= 0; i--) { // O(D) iteraciones | Guarda: O(1) 
      if (_saldos_diarios[i].first == fin_de_dia) { // O(1)
          _saldos_diarios[i].second = _saldo; // O(1)
          encontrado = true; // O(1)
          break; // O(1)
      }
  }

  if (!encontrado) { // O(1)
      _saldos_diarios.push_back({fin_de_dia, _saldo}); // O(1) amortizado
  }

  // Complejidad total del metodo: O(C+D)
  // - 9*O(1) + O(C)*5*O(1) + 3*O(1) + O(C)*2*O(1) + 2*O(1) + O(D)*4*O(1) + 2*O(1)
  // - O(1) + O(C) + O(C) + O(D) = O(max(C,C,D)) = O(C+D)

  // O(D log(D) + C) es una cota superior asintotica de O(C+D).
  // O(C+D) ∈ O(D log(D) + C) si y solo si existen c > 0 y n0 > 0 / 0<= (C+D) <= c(Dlog(D)+C) para todo n>=n0
  // La desigualdad 0 <= C+D vale porque n0 > 0 y la funcion es creciente
  // C+D <= cDlog(D)+C = D<=cDlog(D) = 1<=clog(D) lo cual cumple para D>=3 entonces, existe una constante n0=3 y c=1.
  // En consecuencia, se verifica que: C+D ∈ O(DlogD+C)

}

monto Billetera::saldo() const {
  return _saldo; // O(1)
  // Complejidad total del metodo: O(1)
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  timestamp fin_del_dia = Calendario::fin_del_dia(t); // O(1)

  int izq = 0; // O(1)
  int der = _saldos_diarios.size() - 1; // O(1)
  // Complejidad total del ciclo: O(log(D))
  // - O(log(D)) iteraciones
  // - O(1) operaciones en cada iteracion
  while(izq<=der){ // O(log(D)) | Guarda: O(1) 
    int medio = (izq+der)/2; // O(1)
    if(_saldos_diarios[medio].first == fin_del_dia){ // O(1)
      return _saldos_diarios[medio].second; // O(1)
    }else if(_saldos_diarios[medio].first < fin_del_dia){ // O(1)
      izq = medio + 1; // O(1)
    }else{ 
      der = medio - 1; // O(1)
    }
  }

  if(der>=0){ // O(1)
    return _saldos_diarios[der].second; // O(1)
  }

  return 0; // O(1)

  // Complejidad total del metodo: O(log(D))
  // - 3*O(1) + O(log(D))*6*O(1) + 3*O(1)
  // - O(1) + O(log(D)) = O(max(log(D)), 1) = O(log(D))

}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const {

  vector<Transaccion> ret; // O(1)

  int i = _relevantes.size() - 1; // O(1)
  // Complejidad total del ciclo: O(k)
  // - O(k) iteraciones
  // - O(1) operaciones en cada iteracion
  while(i>=0 && ret.size() < k){ // O(k) iteraciones | Guarda: O(1) 
    ret.push_back(_relevantes[i]);  // O(1) amortizado
    i--;  // O(1)
  }

  return ret; // O(1)

  // Complejidad total del metodo: O(k)
  // - 2*O(1) + O(k)*2*O(1) + O(1)
  // - O(1) + O(k) = O(max(k, 1)) = O(k)
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {

  vector<id_billetera> ret = {}; // O(1)
  int i = 0; // O(1)
  while (i<_transacciones_por_billetera.size() && ret.size() < k) { // k iteraciones | Guarda: O(1) 
    ret.push_back(_transacciones_por_billetera[i].second); // O(1) amortizado
    i++; // O(1)
  }
  return ret; // O(1)

  // Complejidad total del metodo: O(k)
  // - 2*O(1) + O(k)*2*O(1) + O(1)
  // - O(1) + O(k) = O(max(k, 1)) = O(k)
}
