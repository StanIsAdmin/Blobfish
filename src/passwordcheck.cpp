#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

/*Ce programme est un algorithme permettant de récupérer une série (liste) de valeurs allant de 1 à 32 
inclus à partir d'un  mot de passe d'au moins 2 caractères mais pouvant en contenir plus de 32. 
Tous les caractères unicode sont acceptés (je pense !) et l'algorithme vise à constituer la série
de valeurs de façon unique pour chaque mot de passe, en employant tous ses caractères.*/

vector<int> getunval(string pwd);
vector<int> adaptlenght(vector<int> pwdvalues);
vector<int> normval(vector<int> pwdvalues);
void display(vector<int> const &pwdvalues);
int getavg(vector<int> const &pwdvalues);
int make_unique(vector<int> const &pwdvalues, int tempvalue);

const int maxsize = 32; //la taille de la série de valeurs que l'on veut obtenir

int main()
{
	while (true)
	{
		string pwd = "";
		cout << "Password : ";
		getline (cin, pwd);
		vector<int> pwdvalues;
		pwdvalues = getunval(pwd);	//on récupère une liste de valeurs différentes de mm longueur que le mdp
		pwdvalues = adaptlenght(pwdvalues); //on ajuste la longueur à maxsize
		pwdvalues = normval(pwdvalues); //on normalise les valeurs à une série de 0 à maxsize
		display(pwdvalues);
	}
	return 0;
}

vector<int> getunval(string pwd) /*Attribue une valeur différente par caractère
								 du mot de passe, sans se soucier de sa longueur*/
{
	int const pwdsize = pwd.size();
	int tempvalue (0);
	int average (0);
	int supp (0);
	vector<int> pwdvalues(pwdsize);
	for (int i = 0; i < pwdsize; i++)
	{
		tempvalue = int(pwd[i]);
		tempvalue = make_unique(pwdvalues, tempvalue);
		pwdvalues[i] = abs(tempvalue);
	}
	return pwdvalues;
}

vector<int> adaptlenght(vector<int> pwdvalues)/*Adapte la liste à la longueur voulue*/
{
	int i = 0; //index
	int tempvalue = 0;
	while (pwdvalues.size() > maxsize)/*tant que la liste est plus longue que requis, on 
									  additionne la dernière valeur à celle d'index i et
									  puis on élimine la dernière valeur et incrémente i*/
	{
		i = i%maxsize;
		tempvalue = pwdvalues[i] + pwdvalues.back();
		tempvalue = make_unique(pwdvalues, tempvalue);
		pwdvalues[i] = abs(tempvalue);
		pwdvalues.pop_back();
		i ++;
	}
	while (pwdvalues.size() < maxsize) /*tant que la liste est plus courte que requis, on 
									   additionne les valeurs d'index i et i+1 et on insère
									   la valeur résultante en fin de liste, 
									   puis on déplace l'index des valeurs à additionner*/
	{
		i = i%pwdvalues.size();
		tempvalue = pwdvalues[i]+pwdvalues[i+1];
		tempvalue = make_unique(pwdvalues, tempvalue);
		pwdvalues.push_back(abs(tempvalue));
		i ++;
	}
	return pwdvalues;
}

vector<int> normval(vector<int> pwdvalues) /*cherche itérativement les valeurs maximales et les remplace
										   par des valeurs allant de -maxsize à -1, puis change
										   le signe de toutes les valeurs*/
{
	int index = 0;
	int countup = -maxsize;
	while (countup < 0) //Countup sert de compteur pour le nombre d'itérations
	{
		int maxval = 0;
		for (int i = 0; i < maxsize; i++) //Cherche la valeur maximale et retient son index
		{
			if (pwdvalues[i] > maxval){
				index = i;
				maxval = pwdvalues[i];}
		}
		pwdvalues[index] = countup;
		countup += 1;
	}
	for (int i = 0; i < maxsize; i++)//Rend toutes les valeurs positives
	{
		pwdvalues[i] = -1*pwdvalues[i];
	}
	return(pwdvalues);
}

int make_unique(vector<int> const &pwdvalues, int tempvalue) /*Tant que tempvalue est contenu dans la liste
															 on modifie sa valeur selon un algorithme*/
{
	int average = getavg(pwdvalues);
	int supp = 0;
	while (count(pwdvalues.begin(), pwdvalues.end(), tempvalue) != 0) //Tant que la valeur existe dans pwdvalues
	{
		tempvalue = (average*tempvalue + supp)%32000; //Algorithme de modification
		supp += 1; //Cela évite des itérations inutiles et s'assure que les 32000 valeurs soient disponibles
	}
	return tempvalue;
}

int getavg(vector<int> const &pwdvalues) /*Retourne la valeur moyenne de la liste pwdvalues*/
{
	int sum = 0;
	int s = pwdvalues.size();
	for (int i = 0; i < s; i++)
	{
		sum += pwdvalues[i];
	}
	return sum/s;
}

void display(vector<int> const &pwdvalues) /*Affiche le contenu de pwdvalues dans la console*/
{
	int s = pwdvalues.size();
	for (int i = 0; i < s; i++)
	{
		cout << pwdvalues[i] << " ";
	}
	cout << endl;
}
