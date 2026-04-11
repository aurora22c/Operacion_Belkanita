#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

// =========================================================================
// 脕REA DE IMPLEMENTACI脫N DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisi贸n del agente seg煤n el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}

ComportamientoIngeniero::EstadoI ComportamientoIngeniero::NextCasillaIngeniero(const EstadoI &st){
	EstadoI siguiente = st;
	switch (st.site.brujula)
	{
	case norte:
			siguiente.site.f = st.site.f - 1;
			break;
	case noreste:
			siguiente.site.f = st.site.f - 1;
			siguiente.site.c = st.site.c + 1;
			break;
	case este:
			siguiente.site.c = st.site.c + 1;
			break;
	case sureste:
			siguiente.site.f = st.site.f + 1;
			siguiente.site.c = st.site.c + 1;
			break;
	case sur:
			siguiente.site.f = st.site.f + 1;
			break;
	case suroeste:
			siguiente.site.f = st.site.f + 1;
			siguiente.site.c = st.site.c - 1;
			break;
	case oeste:
			siguiente.site.c = st.site.c - 1;
			break;
	case noroeste:
			siguiente.site.f = st.site.f - 1;
			siguiente.site.c = st.site.c - 1;
	}
	return siguiente;
}

/**
* @brief Determina la mejor opci髇 entre las 3 casillas que tiene delante.
* @param i terreno que hay en la posici髇 1 de superficie (45 izq)
* @param c terreno que hay en la posicion 2 de superficie (justo delante)
* @param d terreno que hay en la posici髇 1 de superficie (45 dch)
* @param zap indica si estoy en posesi髇 de las zapatillas
* @return 2 si es mejor WALK, 1 para TURN_SL y 3 para TURN_SR. 0 si no hay nada interesante. 
*/
int VeoCasillaInteresanteI (char i, char c, char d, bool zap)
{  
   if (c == 'U') return 2;
   else if (i == 'U') return 1;
   else if (d == 'U') return 3;
   else if (!zap) {
      if (c == 'D') return 2;
      else if (i == 'D') return 1;
      else if (d == 'D') return 3;
   }
   
   if (c == 'C') return 2;
   else if (i == 'C') return 1;
   else if (d == 'C') return 3;
   else return 0;
}

/**
* @brief Determina si casilla es viable por altura.
* @param casilla tipo de terreno
* @param dif diferencia de altura entre casillas
* @param zap indica si estoy en posesi髇 de las zapatillas
* @return 'P' si no es accesible por altura y casilla en otro caso
*/
char ViablePorAlturaI (char casilla, int dif, bool zap)
{
   if (abs(dif) <= 1 || (zap && abs(dif) <= 2))
      return casilla;
   else 
      return 'P';
}


char ViableI (char casilla, int dif, bool zap, char agente)
{
	 casilla = ViablePorAlturaI(casilla, dif, zap);
	 
	 if (agente == 't')
	 		return 'P';
	 else
	 		return casilla;
}

int ComportamientoIngeniero::EvaluarCasillaI_N0(char casilla, int dif, char agente, int visitas) 
{
	casilla = ViableI(casilla, dif, tiene_zapatillas, agente);
		
	int costo = MAX_COSTO;
	
	if (casilla == 'C' || casilla == 'D') costo = visitas;
	else if (casilla == 'U') costo = visitas + COSTO_U;
	else if (casilla == 'D' && !tiene_zapatillas) costo = visitas + COSTO_D;
	
	return costo;
}
	
// Niveles iniciales (Comportamientos reactivos simples)
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  Action accion = IDLE;
  // El comportamiento de seguir un camino hasta encontrar una planta de T. Residuos
  // Poner el valor de los sensores de visiOn sobre los mapas
  ActualizarMapa(sensores);
  
  // Actualizaci贸n de variables de estado
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
  
  // DefiniciOn del comportamiento 
  if (sensores.superficie[0] == 'U'){ // Lllegue a una 'U'
  	return IDLE; 
  }
  
  explorado[sensores.posF][sensores.posC]++;
  
  EstadoI izda = NextCasillaIngeniero({{sensores.posF, sensores.posC, 
  																		(Orientacion)((sensores.rumbo+7)%8)}, tiene_zapatillas});
  EstadoI dcha = NextCasillaIngeniero({{sensores.posF, sensores.posC, 
  																		(Orientacion)((sensores.rumbo+1)%8)}, tiene_zapatillas});
  EstadoI ctro = NextCasillaIngeniero({{sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas});
  
  int costo_i = EvaluarCasillaI_N0(sensores.superficie[1], sensores.cota[1]-sensores.cota[0], 
  								 								sensores.agentes[1], explorado[izda.site.f][izda.site.c]);
  
  int costo_c = EvaluarCasillaI_N0(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], 
  								 								sensores.agentes[2], explorado[ctro.site.f][ctro.site.c]);
  								 								
  int costo_d = EvaluarCasillaI_N0(sensores.superficie[3], sensores.cota[3]-sensores.cota[0], 
  								 								sensores.agentes[3], explorado[dcha.site.f][dcha.site.c]);
  
	int costo_min = min({costo_c, costo_i, costo_d});
  
  if (costo_min == MAX_COSTO) {
      if (giro45Izq > 4)
      	giro45Izq = -5;
      else if (giro45Izq < 0)
      	accion = TURN_SR;
      else 
      	accion = TURN_SL;
      	
      giro45Izq++;
  } 
  else if (costo_min == costo_c) {
      accion = WALK;
      giro45Izq = 0;
  } 
  else if (costo_min == costo_i) {
      accion = TURN_SL;
      giro45Izq = 0;
  } 
  else {
      accion = TURN_SR;
      giro45Izq = 0;
  }
  
  // Devolver la siguiente acci贸n a hacer
  last_action = accion;
  return accion;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Car谩cter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

int ComportamientoIngeniero::EvaluarCasillaI_N1(char casilla, int dif, char agente, int visitas) 
{
	casilla = ViableI(casilla, dif, tiene_zapatillas, agente);
		
	int costo = MAX_COSTO;
	
	if ( casilla != 'M' && casilla != 'P' && casilla != 'B' ) {
		costo = visitas;
		
		if (casilla == 'D' && !tiene_zapatillas)
			costo += COSTO_D;
		else if ( es_camino(casilla) || casilla == 'S')
			costo += COSTO_C;
	}
	
	return costo;
}

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acci贸n a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  Action accion = IDLE;
  // El comportamiento de seguir un camino hasta encontrar una planta de T. Residuos
  // Poner el valor de los sensores de visiOn sobre los mapas
  ActualizarMapa(sensores);
  
  // Actualizaci贸n de variables de estado
  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
  
  explorado[sensores.posF][sensores.posC]++;
  
  EstadoI izda = NextCasillaIngeniero({{sensores.posF, sensores.posC, 
  																		(Orientacion)((sensores.rumbo+7)%8)}, tiene_zapatillas});
  EstadoI dcha = NextCasillaIngeniero({{sensores.posF, sensores.posC, 
  																		(Orientacion)((sensores.rumbo+1)%8)}, tiene_zapatillas});
  EstadoI ctro = NextCasillaIngeniero({{sensores.posF, sensores.posC, sensores.rumbo}, tiene_zapatillas});
  
  int costo_i = EvaluarCasillaI_N1(sensores.superficie[1], sensores.cota[1]-sensores.cota[0], 
  								 								sensores.agentes[1], explorado[izda.site.f][izda.site.c]);
  
  int costo_c = EvaluarCasillaI_N1(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], 
  								 								sensores.agentes[2], explorado[ctro.site.f][ctro.site.c]);
  								 								
  int costo_d = EvaluarCasillaI_N1(sensores.superficie[3], sensores.cota[3]-sensores.cota[0], 
  								 								sensores.agentes[3], explorado[dcha.site.f][dcha.site.c]);
  
	int costo_min = min({costo_c, costo_i, costo_d});
  
  if (costo_min == MAX_COSTO) {
      if (giro45Izq > 1)
      	giro45Izq = -2;
      else if (giro45Izq < 0)
      	accion = TURN_SR;
      else 
      	accion = TURN_SL;
      	
      giro45Izq++;
  } 
  else if (costo_min == costo_c) {
      accion = WALK;
  } 
  else if (costo_min == costo_i) {
      accion = TURN_SL;
  } 
  else {
      accion = TURN_SR;
  }
  
  // Devolver la siguiente acci贸n a hacer
  last_action = accion;
  return accion;
}




bool ComportamientoIngeniero::CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, 
																												const vector<vector<unsigned char>> &altura){
	EstadoI next = NextCasillaIngeniero(st);
	bool check1 = false, check2 = false, check3 = false;
	check1 = terreno[next.site.f][next.site.c] != 'P' && terreno[next.site.f][next.site.c] != 'M';
	check2 = terreno[next.site.f][next.site.c] != 'B';
	check3 = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) <= 1 ||
					 abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) <= 2 && st.zapatillas;
	return check1 and check2 and check3;
}


ComportamientoIngeniero::EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI & st, 
					const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
	EstadoI next = st;
	switch(accion){
	case WALK:
			if (CasillaAccesibleIngeniero(st,terreno,altura)){
				next = NextCasillaIngeniero(st);
			}
			break;
	case TURN_SR:
			next.site.brujula = (Orientacion) ((next.site.brujula+1)%8);
			break;
	case TURN_SL:
			next.site.brujula = (Orientacion) ((next.site.brujula+7)%8);
			break;
	}
	return next;
}

list<Action> ComportamientoIngeniero::B_Anchura(const EstadoI &inicio, const EstadoI &fin,
																								const vector<vector<unsigned char>> &terreno,
																								const vector<vector<unsigned char>> &altura) {
	NodoI current_node;
	list<NodoI> frontier;
	set<NodoI> explored;
	list<Action> path;
	
	current_node.estado = inicio;
	frontier.push_back(current_node);
	bool SolutionFound = (current_node.estado.site.f == fin.site.f && current_node.estado.site.c == fin.site.c);
	
	while (!SolutionFound && !frontier.empty()) {
		frontier.pop_front();
		explored.insert(current_node);
		
		if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
			current_node.estado.zapatillas = true;
		}
		
		NodoI child_Walk = current_node;
		child_Walk.estado = applyI(WALK, current_node.estado, terreno, altura);
		if (child_Walk.estado.site.f == fin.site.f && child_Walk.estado.site.c == fin.site.c) {
			child_Walk.secuencia.push_back(WALK);
			current_node = child_Walk;
			SolutionFound = true;
		}
		else if (explored.find(child_Walk) == explored.end()) {
			child_Walk.secuencia.push_back(WALK);
			frontier.push_back(child_Walk);
		}
		
		if (!SolutionFound) {
			NodoI child_TurnSR = current_node;
			child_TurnSR.estado = applyI(TURN_SR, current_node.estado, terreno, altura);
			if (explored.find(child_TurnSR) == explored.end()) {
				child_TurnSR.secuencia.push_back(TURN_SR);
				frontier.push_back(child_TurnSR);
			}
			
			NodoI child_TurnSL = current_node;
			child_TurnSL.estado = applyI(TURN_SL, current_node.estado, terreno, altura);
			if (explored.find(child_TurnSL) == explored.end()) {
				child_TurnSL.secuencia.push_back(TURN_SL);
				frontier.push_back(child_TurnSL);
			}
			
		}
		
		if (!SolutionFound && !frontier.empty()) {
			current_node = frontier.front();
			while (explored.find(current_node) != explored.end() && !frontier.empty()) {
				frontier.pop_front();
				current_node = frontier.front();
			}
		}
	}
	
	if (SolutionFound) {
		path = current_node.secuencia;
	}
	
	return path;
}

// Niveles avanzados (Uso de b煤squeda)
/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (b煤squeda).
 * @param sensores Datos actuales de los sensores.
 * @return Acci贸n a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  Action accion = IDLE;
	if (!hayPlan){
		// Invocar al m閠odo de b鷖queda
		EstadoI inicio, fin;
		inicio.site.f = sensores.posF;
		inicio.site.c = sensores.posC;
		inicio.site.brujula = sensores.rumbo;
		inicio.zapatillas = tiene_zapatillas;
		fin.site.f = sensores.BelPosF;
		fin.site.c = sensores.BelPosC;
		plan = B_Anchura(inicio, fin, mapaResultado, mapaCotas);
		VisualizaPlan(inicio.site,plan);
		hayPlan = plan.size() != 0 ;
	}
	if (hayPlan and plan.size()>0){
		accion = plan.front();
		plan.pop_front();
	}
	if (plan.size()== 0){
		hayPlan = false;
	}
	return accion;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acci贸n a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acci贸n a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acci贸n a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acci贸n a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la informaci贸n de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
        mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
      }
    break;
  case noreste:
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
    mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
    mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
    mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
    mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
    mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
    mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
    mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
    mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
    break;
  case este:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
        mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
      }
    break;
  case sureste:
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
    mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
    mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
    mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
    mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
    mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
    mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
    mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
    mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
    mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
    mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
    break;
  case sur:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
        mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
      }
    break;
  case suroeste:
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
    mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
    mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
    mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
    mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
    mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
    mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
    mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
    mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
    break;
  case oeste:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
        mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
      }
    break;
  case noroeste:
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
    mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
    mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
    mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
    mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
    mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
    mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
    mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
    mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
    mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
    mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
    break;
  }
}

/**
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel m谩ximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posici贸n (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal seg煤n la orientaci贸n actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Imprime las coordenadas y operaciones de un plan de tuber铆a.
 *
 * @param plan  Lista de pasos (fila, columna, operaci贸n),
 *              donde operacion = -1 (DIG), operaci贸n = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualizaci贸n en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}

/**
 * @brief Convierte un plan de tuber铆a en la lista de casillas usada
 *        por el sistema de visualizaci贸n.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tuber铆a.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
