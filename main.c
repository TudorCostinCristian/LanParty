#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define L_MAX 128

struct player{
	int xp;
	char *nume, *prenume;
};
typedef struct player PLAYER;

struct team{
	char *nume;
	int size;
	float punctaj;
	PLAYER *player;
	struct team *next;
};
typedef struct team TEAM;

struct match{
	TEAM *team1, *team2;
	struct match *next;
};
typedef struct match MATCH;

struct queue{
	MATCH *rear, *front;
};
typedef struct queue QUEUE;

struct node{
	struct node *left, *right;
	TEAM *team;
};
typedef struct node NODE;

struct avlnode{
	struct avlnode *left, *right;
	int height;
	TEAM *team;
};
typedef struct avlnode AVLNODE;

int EchipeRamase(int n)
{
	if(n < 0) return -1;
	while((n & ( n - 1)) != 0) n = n & (n - 1);
	return n;
}

int CitireJucatori(FILE *in, TEAM *p)
{
	int i, l;
	char nume[L_MAX], prenume[L_MAX];
	for(i = 0; i < p->size; i++)
	{
		fscanf(in, "%s", nume);
		fscanf(in, "%s", prenume);
		l = strlen(nume);
		p->player[i].nume = (char *) malloc(l * sizeof(char) + 1);
		strcpy(p->player[i].nume, nume);
		if(p->player[i].nume == NULL)
		{
			printf("Alocare dinamica esuata!");
			return 0;
		}
		l = strlen(prenume);
		p->player[i].prenume = (char *) malloc(l * sizeof(char) + 1);
		strcpy(p->player[i].prenume, prenume);
		if(p->player[i].prenume == NULL)
		{
			printf("Alocare dinamica esuata!");
			return 0;
		}
		fscanf(in, "%d", &p->player[i].xp);
		p->punctaj += (float)p->player[i].xp;
	}
	p->punctaj /= (float)p->size;
	return 1;
}

float CalculeazaMinim(TEAM *head, int *cnt)
{
	TEAM *p;
	float minim = head->next->punctaj;
	*cnt = 0;
	for(p = head->next; p != NULL; p = p->next)
	{
		if(minim > p->punctaj)
		{
			minim = p->punctaj;
			*cnt = 1;
		}
		else if(minim == p->punctaj) (*cnt)++;
	}
	return minim;
}

int CitireEchipe(FILE *in, TEAM **head, int *n)
{
	TEAM *p, *q;
	char nume[L_MAX];
	int i, j, l;
	fscanf(in, "%d", n);
	*head = (TEAM *) malloc(sizeof(TEAM));
	if(*head == NULL)
	{
		printf("Alocare dinamica esuata!");
		return 0;
	}
	(*head)->next = NULL;
	(*head)->nume = NULL;
	(*head)->size = 0;
	(*head)->punctaj = 0.0;
	(*head)->player = NULL;
	q = NULL;
	for(i = 0; i < *n; i++)
	{
		p = (TEAM *) malloc(sizeof(TEAM));
		if(p == NULL)
		{
			printf("Alocare dinamica esuata!");
			return 0;
		}
		p->nume = NULL;
		p->size = 0;
		p->punctaj = 0.0;
		p->player = NULL;
		(*head)->next = p;
		p->next = q;
		fscanf(in, "%d", &p->size);
		fgets(nume, L_MAX, in);
		l = strlen(nume);
		nume[l-1] = '\0';
		p->nume = (char *) malloc(sizeof(char) * l);
		if(p->nume == NULL)
		{
			printf("Alocare dinamica esuata!");
			return 0;
		}
		strcpy(p->nume, nume);
		p->player = (PLAYER *) malloc(sizeof(PLAYER) * (p->size));
		if(p->player == NULL)
		{
			printf("Alocare dinamica esuata!");
			return 0;
		}
		for(j = 0; j < p->size; j++)
		{
			p->player[j].nume = NULL;
			p->player[j].prenume = NULL;
			p->player[j].xp = 0;
		}
		if(CitireJucatori(in, p) == 0) return 0;
		q = p;
	}
	return 1;
}

void AfisareEchipe(FILE *out,TEAM *head)
{
	while(head->next != NULL)
	{
		head = head->next;
		fprintf(out, "%s\n", head->nume);
	}
}

void CitireCerinte(FILE *cerinta, int *c)
{
	int i, aux;
	*c = 0;
	for(i = 0; i < 5; i++)
	{
		fscanf(cerinta, "%d", &aux);
		(*c) += aux;
	}
}

void EliminaDupa(TEAM *p)
{
	TEAM *q;
	q = p->next;
	p->next = q->next;
	free(q->nume);
	int i;
	for(i = 0; i < q->size; i++)
	{
		free(q->player[i].nume);
		free(q->player[i].prenume);
	}
	free(q->player);
	free(q);
}

void EliminaEchipe(TEAM **head, int *n)
{
	TEAM *p, *q, *start;
	int nr = *n, cnt;
	float minim;
	start = (*head)->next;
	q = *head;
	*n = EchipeRamase(*n);
	minim = CalculeazaMinim(*head, &cnt);
	while(nr != *n)
	{
		for(p = start; p != NULL && nr != *n; p = p->next)
		{
			if(p->punctaj == minim)
			{
				EliminaDupa(q);
				if(*head == q) start = (*head)->next;
				minim = CalculeazaMinim(*head, &cnt);
				if(cnt >= 2 && (*head)->next->punctaj != minim) p = q;
				else
				{
					q = *head;
					nr--;
					break;
				}
				nr--;
			}
			else q = p;
		}
	}
}

int CreeazaCoada(QUEUE **queue)
{
	*queue = (QUEUE *)malloc(sizeof(QUEUE));
	if(*queue == NULL)
	{
		printf("Alocare dinamica esuata!");
		return 0;
	}
	(*queue)->front = NULL;
	(*queue)->rear = NULL;
	return 1;
}

void AdaugaMeciuri(QUEUE *queue, TEAM *head, int k)
{
	TEAM *t;
	MATCH *p;
	if(k == 1) t = head->next;
	else if(k == 2) t = head;
	while(t != NULL)
	{
		p = (MATCH *) malloc(sizeof(MATCH));
		p->next = NULL;
		if(queue->rear == NULL) queue->rear = p;
		else
		{
			queue->rear->next = p;
			queue->rear = p;
		}
		if(queue->front == NULL) queue->front = queue->rear;
		p->team1 = t;
		p->team2 = t->next;
		t = t->next->next;
	}
}

void AfisareMeciuri(FILE *out,QUEUE *queue)
{
	MATCH *p;
	for(p = queue->front; p != NULL; p = p->next)
		fprintf(out, "%-25s -- %25s\n", p->team1->nume, p->team2->nume);
}

void StergeStiva(TEAM **head)
{
	TEAM *p;
	while(*head != NULL)
	{
		p = *head;
		*head = (*head)->next;
		int i;
		for(i = 0; i < p->size; i++)
		{
			free(p->player[i].nume);
			free(p->player[i].prenume);
		}
		free(p->nume);
		free(p->player);
		free(p);
	}
	*head = NULL;
}

void GolesteCoada(QUEUE *queue)
{
	MATCH *p, *q;
	p = queue->front;
	while(p != NULL)
	{
		q = p;
		p = p->next;
		free(q);
	}
	queue->front = NULL;
	queue->rear = NULL;
}

void StergeCoada(QUEUE **queue)
{
	GolesteCoada(*queue);
	free(*queue);
	*queue = NULL;
}

void StergeLista(TEAM **head)
{
	TEAM *p, *q;
	p = (*head)->next;
	while(p != NULL)
	{
		q = p;
		p = p->next;
		int i;
		for(i = 0; i < q->size; i++)
		{
			free(q->player[i].nume);
			free(q->player[i].prenume);
		}
		free(q->nume);
		free(q->player);
		free(q);
	}
	free(*head);
}

void CreeazaStive(QUEUE *queue, TEAM **topWinners, TEAM **topLosers)
{
	int i;
	MATCH *p;
	for(p = queue->front; p != NULL; p = p->next)
	{
		if(p->team1->punctaj >= p->team2->punctaj)
		{
			p->team1->next = *topWinners;
			*topWinners = p->team1;
			p->team1->punctaj += 1.0;
			for(i = 0; i < p->team1->size; i++)
			{
				p->team1->player[i].xp += 1.0;
			}
			p->team2->next = *topLosers;
			*topLosers = p->team2;
		}
		else
		{
			p->team2->next = *topWinners;
			*topWinners = p->team2;
			p->team2->punctaj += 1.0;
			for(i = 0; i < p->team2->size; i++)
			{
				p->team2->player[i].xp += 1.0;
			}
			p->team1->next = *topLosers;
			*topLosers = p->team1;
		}
	}
	GolesteCoada(queue);
}

void AfisareCastigatori(FILE *out, TEAM *topWinners)
{
	while(topWinners != NULL)
	{
		fprintf(out, "%-25s --- %.2f\n", topWinners->nume, topWinners->punctaj);
		topWinners = topWinners->next;
	}
}

int DeschideFisiere(FILE **in, FILE **out, FILE **cerinta, char *argv[])
{
	if((*cerinta = fopen(argv[1], "r")) == NULL)
	{
		printf("Nu s-a putut deschide fisierul input!");
		return 0;
	}
	if((*in = fopen(argv[2], "r")) == NULL)
	{
		printf("Nu s-a putut deschide fisierul input!");
		return 0;
	}
	if((*out = fopen(argv[3], "w")) == NULL)
	{
		printf("Nu s-a putut deschide fisierul output!");
		return 0;
	}
	return 1;
}

NODE* CreeazaNod(TEAM *p)
{
	int i;
	NODE* new;
	new = (NODE *) malloc(sizeof(NODE));
	new->left = NULL;
	new->right = NULL;
	new->team = (TEAM *) malloc(sizeof(TEAM));
	new->team->next = NULL;
	new->team->nume = (char *) malloc(sizeof(char)*strlen(p->nume) + 1);
	strcpy(new->team->nume, p->nume);
	new->team->punctaj = p->punctaj;
	new->team->size = p->size;
	new->team->player = (PLAYER *) malloc(sizeof(PLAYER)*(p->size));
	for(i = 0; i < new->team->size; i++)
	{
		new->team->player[i].nume = (char *) malloc(sizeof(char)*strlen(p->player[i].nume) + 1);
		strcpy(new->team->player[i].nume, p->player[i].nume);
		new->team->player[i].prenume = (char *) malloc(sizeof(char)*strlen(p->player[i].prenume) + 1);
		strcpy(new->team->player[i].prenume, p->player[i].prenume);
		new->team->player[i].xp = p->player[i].xp;
	}
	return new;
}

void AdaugaNod(NODE *root, NODE *p)
{
	if(p->team->punctaj < root->team->punctaj)
	{
		if(root->left == NULL)
		{
			root->left = p;
			return;
		}
		AdaugaNod(root->left, p);
	}
	else if(p->team->punctaj > root->team->punctaj)
	{
		if(root->right == NULL)
		{
			root->right = p;
			return;
		}
		AdaugaNod(root->right, p);
	}
	else
	{
		int i;
		for(i = 0; i < p->team->size; i++)
		{
			free(p->team->player[i].nume);
			free(p->team->player[i].prenume);
		}
		free(p->team->nume);
		free(p->team->player);
		free(p->team);
		free(p);
	}
}

void AfiseazaClasament(FILE *out, NODE *root)
{
	if(root == NULL) return;
	AfiseazaClasament(out, root->right);
	fprintf(out, "%-20s -- %.2f\n", root->team->nume, root->team->punctaj);
	AfiseazaClasament(out, root->left);
}

void ElibereazaMemorieBST(NODE *root)
{
	int i;
	if(root == NULL) return;
	ElibereazaMemorieBST(root->left);
	ElibereazaMemorieBST(root->right);
	free(root->team->nume);
	for(i = 0; i < root->team->size; i++)
	{
		free(root->team->player[i].nume);
		free(root->team->player[i].prenume);
	}
	free(root->team->player);
	free(root->team);
	free(root);
}

AVLNODE* CreeazaNodAVL(TEAM *p)
{
	int i;
	AVLNODE* new;
	new = (AVLNODE *) malloc(sizeof(AVLNODE));
	new->left = NULL;
	new->right = NULL;
	new->height = 0;
	new->team = (TEAM *) malloc(sizeof(TEAM));
	new->team->next = NULL;
	new->team->nume = (char *) malloc(sizeof(char)*strlen(p->nume) + 1);
	strcpy(new->team->nume, p->nume);
	new->team->punctaj = p->punctaj;
	new->team->size = p->size;
	new->team->player = (PLAYER *) malloc(sizeof(PLAYER)*(p->size));
	for(i = 0; i < new->team->size; i++)
	{
		new->team->player[i].nume = (char *) malloc(sizeof(char)*strlen(p->player[i].nume) + 1);
		strcpy(new->team->player[i].nume, p->player[i].nume);
		new->team->player[i].prenume = (char *) malloc(sizeof(char)*strlen(p->player[i].prenume) + 1);
		strcpy(new->team->player[i].prenume, p->player[i].prenume);
		new->team->player[i].xp = p->player[i].xp;
	}
	return new;
}

int AVLFactorEchilibru(AVLNODE *root)
{
	int h1, h2;
	if(root->left == NULL) h1 = -1;
	else h1 = root->left->height;
	if(root->right == NULL) h2 = -1;
	else h2 = root->right->height;
	return h1 - h2;
}

AVLNODE* RotireLaDreapta(AVLNODE *root)
{
	AVLNODE *p, *q;
	p = root->left;
	q = p->right;
	p->right = root;
	root->left = q;
	if(root->left != NULL && root->right != NULL)
	{
		if(root->left->height > root->right->height) root->height = root->left->height + 1;
		else root->height = root->right->height + 1;
	}
	else if(root->left != NULL)
		root->height = root->left->height + 1;
	else if(root->right != NULL)
		root->height = root->right->height + 1;
	else root->height = 0;
	if(p->left != NULL && p->right != NULL)
	{
		if(p->left->height > p->right->height) p->height = p->left->height + 1;
		else p->height = p->right->height + 1;
	}
	else if(p->left != NULL)
		p->height = p->left->height + 1;
	else if(p->right != NULL)
		p->height = p->right->height + 1;
	else p->height = 0;

	return p;
}

AVLNODE* RotireLaStanga(AVLNODE *root)
{
	AVLNODE *p, *q;
	p = root->right;
	q = p->left;
	p->left = root;
	root->right = q;
	if(root->left != NULL && root->right != NULL)
	{
		if(root->left->height > root->right->height) root->height = root->left->height + 1;
		else root->height = root->right->height + 1;
	}
	else if(root->left != NULL)
		root->height = root->left->height + 1;
	else if(root->right != NULL)
		root->height = root->right->height + 1;
	else root->height = 0;
	if(p->left != NULL && p->right != NULL)
	{
		if(p->left->height > p->right->height) p->height = p->left->height + 1;
		else p->height = p->right->height + 1;
	}
	else if(p->left != NULL)
		p->height = p->left->height + 1;
	else if(p->right != NULL)
		p->height = p->right->height + 1;
	else p->height = 0;

	return p;
}

AVLNODE* RotireStangaDreapta(AVLNODE* root)
{
	root->left = RotireLaStanga(root->left);
	return RotireLaDreapta(root);
}

AVLNODE* RotireDreaptaStanga(AVLNODE* root)
{
	root->right = RotireLaDreapta(root->right);
	return RotireLaStanga(root);
}

int AVLInaltimeNod(AVLNODE *root)
{
	if(root == NULL) return -1;
	else return root->height;
}

AVLNODE* AdaugaNodAVL(AVLNODE *root, AVLNODE *p)
{
	if(p->team->punctaj < root->team->punctaj)
	{
		if(root->left == NULL)
		{
			root->left = p;
			int h = root->left->height + 1;
			if(h > root->height) root->height = h;
			return root;
		}
		root->left = AdaugaNodAVL(root->left, p);
		int h = root->left->height + 1;
		if(h > root->height) root->height = h;
	}
	else if(p->team->punctaj > root->team->punctaj)
	{
		if(root->right == NULL)
		{
			root->right = p;
			int h = root->right->height + 1;
			if(h > root->height) root->height = h;
			return root;
		}
		root->right = AdaugaNodAVL(root->right, p);
		int h = root->right->height + 1;
		if(h > root->height) root->height = h;
	}
	else
	{
		int i;
		for(i = 0; i < p->team->size; i++)
		{
			free(p->team->player[i].nume);
			free(p->team->player[i].prenume);
		}
		free(p->team->nume);
		free(p->team->player);
		free(p->team);
		free(p);
	}
	int f = AVLFactorEchilibru(root);
	if(f==2 || f==-2)
	{
		if(AVLInaltimeNod(root->left) > AVLInaltimeNod(root->right))
		{
			if(AVLInaltimeNod(root->left->right) < AVLInaltimeNod(root->left->left))
			{
				root = RotireLaDreapta(root);
				return root;
			}
			else
			{
				root = RotireStangaDreapta(root);
				return root;
			}
		}
		else
		{
			if(AVLInaltimeNod(root->right->left) < AVLInaltimeNod(root->right->right))
			{
				root = RotireLaStanga(root);
				return root;
			}
			else
			{
				root = RotireDreaptaStanga(root);
				return root;
			}
		}
	}
	return root;
}

void AfiseazaClasamentAVL(FILE *out, AVLNODE *root)
{
	if(root == NULL) return;
	AfiseazaClasamentAVL(out, root->right);
	fprintf(out, "%-20s -- %.2f\n", root->team->nume, root->team->punctaj);
	AfiseazaClasamentAVL(out, root->left);
}
/*
void AfisareNivel2AVL(FILE *out, AVLNODE *root)
{
	if(root->left != NULL)
	{
		if(root->left->left != NULL) fprintf(out, "%s\n", root->left->left->team->nume);
		if(root->left->right != NULL) fprintf(out, "%s\n", root->left->right->team->nume);
	}
	if(root->right != NULL)
	{
		if(root->right->left != NULL) fprintf(out, "%s\n", root->right->left->team->nume);
		if(root->right->right != NULL) fprintf(out, "%s\n", root->right->right->team->nume);
	}
}
*/

void ElibereazaMemorieAVL(AVLNODE *root)
{
	int i;
	if(root == NULL) return;
	ElibereazaMemorieAVL(root->left);
	ElibereazaMemorieAVL(root->right);
	free(root->team->nume);
	for(i = 0; i < root->team->size; i++)
	{
		free(root->team->player[i].nume);
		free(root->team->player[i].prenume);
	}
	free(root->team->player);
	free(root->team);
	free(root);
}

void Rezolva1(FILE *out, TEAM **head, QUEUE **queue)
{
	AfisareEchipe(out, *head);
	StergeCoada(queue);
	StergeLista(head);
}

void Rezolva2(FILE *out, TEAM **head, QUEUE **queue, int *n)
{
	EliminaEchipe(head, n);
	AfisareEchipe(out, *head);
	StergeCoada(queue);
	StergeLista(head);
}

int Rezolva3(FILE *out, TEAM **head, QUEUE **queue, int *n, TEAM **topWinners, TEAM **topLosers)
{

	EliminaEchipe(head, n);
	AfisareEchipe(out, *head);
	AdaugaMeciuri(*queue, *head, 1);
	free(*head);
	AfisareMeciuri(out, *queue);
	CreeazaStive(*queue, topWinners, topLosers);
	StergeStiva(topLosers);
	AfisareCastigatori(out, *topWinners);
	(*n)/=2;
	while(*n > 1)
	{
		StergeCoada(queue);
		if(CreeazaCoada(queue) == 0) return 0;
		AdaugaMeciuri(*queue, *topWinners, 2);
		AfisareMeciuri(out, *queue);
		*topWinners = NULL;
		CreeazaStive(*queue, topWinners, topLosers);
		StergeStiva(topLosers);
		AfisareCastigatori(out, *topWinners);
		*n = (*n)/2;
	}
	StergeStiva(topWinners);
	StergeStiva(topLosers);
	StergeCoada(queue);
	return 1;
}

int Rezolva4(FILE *out, TEAM **head, QUEUE **queue, int *n, TEAM **topWinners, TEAM **topLosers)
{
	NODE *root = NULL;
	TEAM *p;
	EliminaEchipe(head, n);
	AfisareEchipe(out, *head);
	AdaugaMeciuri(*queue, *head, 1);
	free(*head);
	AfisareMeciuri(out, *queue);
	CreeazaStive(*queue, topWinners, topLosers);
	StergeStiva(topLosers);
	AfisareCastigatori(out, *topWinners);
	(*n)/=2;
	while(*n > 8)
	{
		StergeCoada(queue);
		if(CreeazaCoada(queue) == 0) return 0;
		AdaugaMeciuri(*queue, *topWinners, 2);
		AfisareMeciuri(out, *queue);
		*topWinners = NULL;
		CreeazaStive(*queue, topWinners, topLosers);
		StergeStiva(topLosers);
		AfisareCastigatori(out, *topWinners);
		*n = (*n)/2;
	}
	while(*n > 1)
	{
		StergeCoada(queue);
		if(CreeazaCoada(queue) == 0) return 0;
		AdaugaMeciuri(*queue, *topWinners, 2);
		AfisareMeciuri(out, *queue);
		*topWinners = NULL;
		CreeazaStive(*queue, topWinners, topLosers);
		for(p = *topLosers; p != NULL; p = p->next)
		{
			if(root == NULL) root = CreeazaNod(p);
			else AdaugaNod(root, CreeazaNod(p));
		}
		StergeStiva(topLosers);
		AfisareCastigatori(out, *topWinners);
		*n = (*n)/2;
	}
	AdaugaNod(root, CreeazaNod(*topWinners));
	StergeStiva(topWinners);
	StergeStiva(topLosers);
	StergeCoada(queue);
	AfiseazaClasament(out, root);
	ElibereazaMemorieBST(root);
	return 1;
}

int Rezolva5(FILE *out, TEAM **head, QUEUE **queue, int *n, TEAM **topWinners, TEAM **topLosers)
{
	AVLNODE *root = NULL;
	TEAM *p;
	EliminaEchipe(head, n);
	AfisareEchipe(out, *head);
	AdaugaMeciuri(*queue, *head, 1);
	free(*head);
	AfisareMeciuri(out, *queue);
	CreeazaStive(*queue, topWinners, topLosers);
	
	StergeStiva(topLosers);
	AfisareCastigatori(out, *topWinners);
	(*n)/=2;
	while(*n > 8)
	{
		StergeCoada(queue);
		if(CreeazaCoada(queue) == 0) return 0;
		AdaugaMeciuri(*queue, *topWinners, 2);
		AfisareMeciuri(out, *queue);
		*topWinners = NULL;
		CreeazaStive(*queue, topWinners, topLosers);
		StergeStiva(topLosers);
		AfisareCastigatori(out, *topWinners);
		*n = (*n)/2;
	}
	while(*n > 1)
	{
		StergeCoada(queue);
		if(CreeazaCoada(queue) == 0) return 0;
		AdaugaMeciuri(*queue, *topWinners, 2);
		AfisareMeciuri(out, *queue);
		*topWinners = NULL;
		CreeazaStive(*queue, topWinners, topLosers);
		for(p = *topLosers; p != NULL; p = p->next)
		{
			if(root == NULL) root = CreeazaNodAVL(p);
			else root = AdaugaNodAVL(root, CreeazaNodAVL(p));
		}
		StergeStiva(topLosers);
		AfisareCastigatori(out, *topWinners);
		*n = (*n)/2;
	}
	root = AdaugaNodAVL(root, CreeazaNodAVL(*topWinners));
	StergeStiva(topWinners);
	StergeStiva(topLosers);
	StergeCoada(queue);
	AfiseazaClasamentAVL(out, root);
	//AfisareNivel2AVL(out, root);
	ElibereazaMemorieAVL(root);
	return 1;
}

int main(int argc, char *argv[])
{
	FILE *in, *out, *cerinta;
	int n, c;
	TEAM *head = NULL, *topWinners = NULL, *topLosers = NULL;
	QUEUE *queue = NULL;
	if(DeschideFisiere(&in, &out, &cerinta, argv) == 0) return 0;
	if(CreeazaCoada(&queue) == 0) return 0;
	CitireEchipe(in, &head, &n);
	CitireCerinte(cerinta, &c);
	switch(c)
	{
		case 1: 
		{
			Rezolva1(out, &head, &queue); 
			break;
		}
		case 2: 
		{
			Rezolva2(out, &head, &queue, &n);
			break;
		}
		case 3:
		{
			if(Rezolva3(out, &head, &queue, &n, &topWinners, &topLosers) == 0) return 0;
			break;
		}
		case 4:
		{
			if(Rezolva4(out, &head, &queue, &n, &topWinners, &topLosers) == 0) return 0;
			break;
		}
		case 5:
		{
			if(Rezolva5(out, &head, &queue, &n, &topWinners, &topLosers) == 0) return 0;
			break;
		}
		default: break;
	}
	fclose(in);
	fclose(out);
	fclose(cerinta);
	return 0;
}