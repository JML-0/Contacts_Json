#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

typedef struct JsonData
{
	FILE            *fp;
	char            buffer[1024];
	struct          json_object *parsed_json;
	struct          json_object *contacts;
	struct          json_object *contact;
}JsonData;

typedef struct ContactData
{
	struct          json_object *name;
	struct          json_object *age;
	struct          json_object *address;
	struct          json_object *tel;
}ContactData;

/*
 * Save a Contact
 */
void SaveContact(FILE *File, json_object *value)
{
	File = fopen("contacts.json", "w+");

	/*Json object*/
  	json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "contacts", value);

	fputs(json_object_to_json_string(jobj), File);

	fclose(File);
}

/*
 * Check id
 */
int CheckContactId(JsonData *File, int idx)
{
	if (json_object_array_get_idx(File->contacts, idx - 1) == NULL) return 0;
	return 1;
}

/*
 * fgets functions (remove \n)
 */
int NewFgets(char *value, int size)
{
    char *Enter = NULL;
 
    if (fgets(value, size, stdin) != NULL)
    {
        Enter = strchr(value, '\n');
        if (Enter != NULL)
        {
            *Enter = '\0';
        }
        return 1;
    }
    else return 0; // error
}

/*
 * Check Contact Name
 * @return: Contact Id or -1 (NULL)
 */
int CheckContactName(JsonData *File, const char *name)
{
	size_t n_contacts = json_object_array_length(File->contacts);

	json_object *contact;
	ContactData contactdata;

	for(size_t i = 0; i < n_contacts; i++) 
	{
		contact = json_object_array_get_idx(File->contacts, i);
		json_object_object_get_ex(contact, "name", &contactdata.name);

		if (strcmp(json_object_get_string(contactdata.name), name) == 0) return i + 1;
	}

	return -1;
}

/*
 * Show all contacts
 */
void ShowAll(JsonData *File, ContactData *Contact)
{
	size_t n_contacts = json_object_array_length(File->contacts);
	printf("Il y a %lu contact(s)\n", n_contacts);

	for(size_t i = 0; i < n_contacts; i++) 
	{
		File->contact = json_object_array_get_idx(File->contacts, i);
		json_object_object_get_ex(File->contact, "name", &Contact->name);
		json_object_object_get_ex(File->contact, "age", &Contact->age);
		json_object_object_get_ex(File->contact, "address", &Contact->address);
		json_object_object_get_ex(File->contact, "tel", &Contact->tel);

		printf("---[Contact n°%lu]---\n", i + 1);
		printf("Nom: %s\n", json_object_get_string(Contact->name));
		printf("Age: %d\n", json_object_get_int(Contact->age));
		printf("Adresse: %s\n", json_object_get_string(Contact->address));
		printf("Téléphone: %s\n", json_object_get_string(Contact->tel));
	}
}

/*
 * Edit a Contact
 */
void EditContact(JsonData *File, int idx)
{
	if (CheckContactId(File, idx))
	{
		char name[101], address[512], tel[11];
		int age;

		printf("Nom : ");
		getchar();
		NewFgets(name, 101);
		printf("Age : ");
		scanf("%d", &age);
		printf("Adresse : ");
		getchar();
		NewFgets(address, 512);
		printf("Téléphone : ");
		scanf("%s", tel);

		/*Json object*/
		json_object *jobj = json_object_new_object();

		/*Json data*/
		json_object *jname = json_object_new_string(name);
		json_object *jage = json_object_new_int(age);
		json_object *jaddress = json_object_new_string(address);
		json_object *jtel = json_object_new_string(tel);

		/*Add Data in Object */
		json_object_object_add(jobj, "name", jname);
		json_object_object_add(jobj, "age", jage);
		json_object_object_add(jobj, "address", jaddress);
		json_object_object_add(jobj, "tel", jtel);

		json_object_array_put_idx(File->contacts, idx - 1, jobj);

		SaveContact(File->fp, File->contacts);

		printf("Contact modifié avec succès !\n");
	}
	else printf("Le numéro n'éxiste pas\n");
}

/*
 * Create a new Contact
 */
void CreateNewContact(JsonData *File, const char* name, int age, const char* address, const char* tel)
{
	int CheckContactNameValue = CheckContactName(File, name);

	if (CheckContactNameValue == -1)
	{
		/*Json object*/
		json_object *jobj = json_object_new_object();

		/*Json data*/
		json_object *jname = json_object_new_string(name);
		json_object *jage = json_object_new_int(age);
		json_object *jaddress = json_object_new_string(address);
		json_object *jtel = json_object_new_string(tel);

		/*Add Data in Object */
		json_object_object_add(jobj, "name", jname);
		json_object_object_add(jobj, "age", jage);
		json_object_object_add(jobj, "address", jaddress);
		json_object_object_add(jobj, "tel", jtel);

		/*Add Object in Array */
		json_object_array_add(File->contacts, jobj);

		SaveContact(File->fp, File->contacts);

		printf("Nouveau contact ajouté avec succès !\n");
	}
	else
	{
		int value = 0;
		printf("Un contact éxiste déjà avec ce nom, souhaitez-vous le modifier ? (1:Oui/2:Non)");
		scanf("%d", &value);
		if (value == 1) EditContact(File, CheckContactNameValue);
	}
}

/*
 * Delete a Contact
 */
void DeleteContact(JsonData *File, int idx)
{
	if (CheckContactId(File, idx))
	{
		json_object_array_del_idx(File->contacts, idx - 1, 1);
		SaveContact(File->fp, File->contacts);

		printf("Contact supprimé avec succès !\n");
	}
	else printf("Le numéro n'éxiste pas\n");
}

void Menu(JsonData *File, ContactData *Contact)
{
	int value = 0;

	printf("-----MENU-----\n");
	printf("1 - Afficher tous les contacts\n");
	printf("2 - Ajouter un nouveau contact\n");
	printf("3 - Modifier un contact\n");
	printf("4 - Supprimer un contact\n");
	printf("5 - Quitter\n\n");
	scanf("%d", &value); /* Choice */

	char name[101], address[512], tel[11];
	int age;
	switch (value)
	{
		case 1:
			ShowAll(File, Contact);
			break;

		case 2:
			printf("Nom : ");
			getchar();
			NewFgets(name, 101);
			printf("Age : ");
			scanf("%d", &age);
			printf("Adresse : ");
			getchar();
			NewFgets(address, 512);
			printf("Téléphone : ");
			scanf("%s", tel);
			CreateNewContact(File, name, age, address, tel);
			break;

		case 3:
			printf("Numéro du contact à modifier : ");
			scanf("%d", &value);
			EditContact(File, value);
			break;

		case 4:
			printf("Numéro du contact à supprimer : ");
			scanf("%d", &value);
			DeleteContact(File, value);
			break;

		case 5:
			exit(0);
			break;

		default:
			break;
	}
	printf("Afficher de nouveau le menu ? (1:Oui/2:Non)");
	scanf("%d", &value);
	if(value == 1) Menu(File, Contact);
}

int main(int argc, char **argv) 
{
	JsonData File;

	File.fp = fopen("contacts.json", "r");

	if (File.fp == NULL)
	{
		fclose(File.fp);
		File.fp = fopen("contacts.json", "w");
		fputs("{ \"contacts\": [] }", File.fp);
		fclose(File.fp);
	} 
	
	File.fp = fopen("contacts.json", "r");
	fread(File.buffer, 1024, 1, File.fp);
	fclose(File.fp);
	File.parsed_json = json_tokener_parse(File.buffer);
	json_object_object_get_ex(File.parsed_json, "contacts", &File.contacts);

	ContactData Contact;
	Menu(&File, &Contact);
}