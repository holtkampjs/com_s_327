#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <list>
#include <new>
#include <ostream>
#include <string>

using namespace std;

class Pokemon {
private:
  int id;
  string identifier;
  int species_id;
  int height;
  int weight;
  int base_experience;
  int order;
  int is_default;

public:
  Pokemon(int id, string identifier, int species_id, int height, int weight,
          int base_experience, int order, int is_default) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->species_id = species_id ? species_id : -1;
    this->height = height ? height : -1;
    this->weight = weight ? weight : -1;
    this->base_experience = base_experience ? base_experience : -1;
    this->order = order ? order : -1;
    this->is_default = is_default ? is_default : -1;
  }

  Pokemon(Pokemon *&p) {
    this->id = p->id;
    this->identifier = p->identifier;
    this->species_id = p->species_id;
    this->height = p->height;
    this->weight = p->weight;
    this->base_experience = p->base_experience;
    this->order = p->order;
    this->is_default = p->is_default;
  }

  friend std::ostream &operator<<(std::ostream &os, const Pokemon &p) {
    os << p.id << ",";
    os << p.identifier << ",";
    os << p.species_id << ",";
    os << p.height << ",";
    os << p.weight << ",";
    os << p.base_experience << ",";
    os << p.order << ",";
    os << p.is_default;
    return os;
  }
};

class Moves {
private:
  int id;
  string identifier;
  int generation_id;
  int type_id;
  int power;
  int pp;
  int accuracy;
  int priority;
  int target_id;
  int damage_class_id;
  int effect_id;
  int effect_chance;
  int contest_type_id;
  int contest_effect_id;
  int super_contest_effect_id;

public:
  Moves(int id, string identifier, int generation_id, int type_id, int power,
        int pp, int accuracy, int priority, int target_id, int damage_class_id,
        int effect_id, int effect_chance, int contest_type_id,
        int contest_effect_id, int super_contest_effect_id) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->generation_id = generation_id ? generation_id : -1;
    this->type_id = type_id ? type_id : -1;
    this->power = power ? power : -1;
    this->pp = pp ? pp : -1;
    this->accuracy = accuracy ? accuracy : -1;
    this->priority = priority ? priority : -1;
    this->target_id = target_id ? target_id : -1;
    this->damage_class_id = damage_class_id ? damage_class_id : -1;
    this->effect_id = effect_id ? effect_id : -1;
    this->effect_chance = effect_chance ? effect_chance : -1;
    this->contest_type_id = contest_type_id ? contest_type_id : -1;
    this->contest_effect_id = contest_effect_id ? contest_effect_id : -1;
    this->super_contest_effect_id =
        super_contest_effect_id ? super_contest_effect_id : -1;
  }

  Moves(Moves *&m) {
    this->id = m->id;
    this->identifier = m->identifier;
    this->generation_id = m->generation_id;
    this->type_id = m->type_id;
    this->power = m->power;
    this->pp = m->pp;
    this->accuracy = m->accuracy;
    this->priority = m->priority;
    this->target_id = m->target_id;
    this->damage_class_id = m->damage_class_id;
    this->effect_id = m->effect_id;
    this->effect_chance = m->effect_chance;
    this->contest_type_id = m->contest_type_id;
    this->contest_effect_id = m->contest_effect_id;
    this->super_contest_effect_id = m->super_contest_effect_id;
  }
};

class PokemonMoves {
private:
  int pokemon_id;
  int version_group_id;
  int move_id;
  int pokemon_move_method_id;
  int level;
  int order;

public:
  PokemonMoves(int pokemon_id, int version_group_id, int move_id,
               int pokemon_move_method_id, int level, int order) {
    this->pokemon_id = pokemon_id ? pokemon_id : -1;
    this->version_group_id = version_group_id ? version_group_id : -1;
    this->move_id = move_id ? move_id : -1;
    this->pokemon_move_method_id =
        pokemon_move_method_id ? pokemon_move_method_id : -1;
    this->level = level ? level : -1;
    this->order = order ? order : -1;
  }

  PokemonMoves(PokemonMoves *&pm) {
    this->pokemon_id = pm->pokemon_id;
    this->version_group_id = pm->version_group_id;
    this->move_id = pm->move_id;
    this->pokemon_move_method_id = pm->pokemon_move_method_id;
    this->level = pm->level;
    this->order = pm->order;
  }
};

class PokemonSpecies {
private:
  int id;
  string identifier;
  int generation_id;
  int evolves_from_species_id;
  int evolution_chain_id;
  int color_id;
  int shape_id;
  int habitat_id;
  int gender_rate;
  int capture_rate;
  int base_happiness;
  int is_baby;
  int hatch_counter;
  int has_gender_differences;
  int growth_rate_id;
  int forms_switchable;
  int is_legendary;
  int is_mythical;
  int order;
  int conquest_order;

public:
  PokemonSpecies(int id, string identifier, int generation_id,
                 int evolves_from_species_id, int evolution_chain_id,
                 int color_id, int shape_id, int habitat_id, int gender_rate,
                 int capture_rate, int base_happiness, int is_baby,
                 int hatch_counter, int has_gender_differences,
                 int growth_rate_id, int forms_switchable, int is_legendary,
                 int is_mythical, int order, int conquest_order) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->generation_id = generation_id ? generation_id : -1;
    this->evolves_from_species_id =
        evolves_from_species_id ? evolves_from_species_id : -1;
    this->evolution_chain_id = evolution_chain_id ? evolution_chain_id : -1;
    this->color_id = color_id ? color_id : -1;
    this->shape_id = shape_id ? shape_id : -1;
    this->habitat_id = habitat_id ? habitat_id : -1;
    this->gender_rate = gender_rate ? gender_rate : -1;
    this->capture_rate = capture_rate ? capture_rate : -1;
    this->base_happiness = base_happiness ? base_happiness : -1;
    this->is_baby = is_baby ? is_baby : -1;
    this->hatch_counter = hatch_counter ? hatch_counter : -1;
    this->has_gender_differences =
        has_gender_differences ? has_gender_differences : -1;
    this->growth_rate_id = growth_rate_id ? growth_rate_id : -1;
    this->forms_switchable = forms_switchable ? forms_switchable : -1;
    this->is_legendary = is_legendary ? is_legendary : -1;
    this->is_mythical = is_mythical ? is_mythical : -1;
    this->order = order ? order : -1;
    this->conquest_order = conquest_order ? conquest_order : -1;
  }

  PokemonSpecies(PokemonSpecies *&ps) {
    this->id = ps->id;
    this->identifier = ps->identifier;
    this->generation_id = ps->generation_id;
    this->evolves_from_species_id = ps->evolves_from_species_id;
    this->evolution_chain_id = ps->evolution_chain_id;
    this->color_id = ps->color_id;
    this->shape_id = ps->shape_id;
    this->habitat_id = ps->habitat_id;
    this->gender_rate = ps->gender_rate;
    this->capture_rate = ps->capture_rate;
    this->base_happiness = ps->base_happiness;
    this->is_baby = ps->is_baby;
    this->hatch_counter = ps->hatch_counter;
    this->has_gender_differences = ps->has_gender_differences;
    this->growth_rate_id = ps->growth_rate_id;
    this->forms_switchable = ps->forms_switchable;
    this->is_legendary = ps->is_legendary;
    this->is_mythical = ps->is_mythical;
    this->order = ps->order;
    this->conquest_order = ps->conquest_order;
  }
};

class Experience {
private:
  int growth_rate_id;
  int level;
  int experience;

public:
  Experience(int growth_rate_id, int level, int experience) {
    this->growth_rate_id = growth_rate_id ? growth_rate_id : -1;
    this->level = level ? level : -1;
    this->experience = experience ? experience : -1;
  }

  Experience(Experience *&e) {
    this->growth_rate_id = e->growth_rate_id;
    this->level = e->level;
    this->experience = e->experience;
  }
};

class TypeNames {
private:
  int type_id;
  int local_language;
  string name;

public:
  TypeNames(int type_id, int local_language, string name) {
    this->type_id = type_id ? type_id : -1;
    this->local_language = local_language ? local_language : -1;
    this->name = name;
  }

  TypeNames(TypeNames *&tn) {
    this->type_id = tn->type_id;
    this->local_language = tn->local_language;
    this->name = tn->name;
  }
};

class Database {
private:
public:
  list<Pokemon> *pokemon;
  list<Moves> *moves;
  list<PokemonMoves> *pokemonMoves;
  list<PokemonSpecies> *pokemonSpecies;
  list<Experience> *experience;
  list<TypeNames> *typeNames;

  Database() {
    pokemon = new list<Pokemon>();
    moves = new list<Moves>();
    pokemonMoves = new list<PokemonMoves>();
    pokemonSpecies = new list<PokemonSpecies>();
    experience = new list<Experience>();
    typeNames = new list<TypeNames>();
  }

  ~Database() {
    // TODO: deallocate all lists
  }

  void printPokemon() {
    list<Pokemon>::iterator itr;
    for (itr = pokemon->begin(); itr != pokemon->end(); ++itr)
      cout << *itr << endl;
  }
};

class CsvParser {
private:
  Database *db;

  string getPath(int attempt, string filename) {
    switch (attempt) {
    case 0:
      cout << "Looking in: ";
      return "/share/cs327/pokedex/pokedex/data/csv/" + filename + ".csv";
    case 1:
      string home_dir(getenv("HOME"));
      cout << "Looking in: ";
      if (home_dir.empty())
        break;
      return home_dir + "/.poke327/pokedex/pokedex/data/csv/" + filename +
             ".csv";
    }

    return "";
  }

  int addPokemon(char *str) {
    char *token;
    Pokemon *p;
    int id;
    string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;

    token = strtok(str, ",");
    id = atoi(token);
    identifier = strtok(NULL, ",");
    token = strtok(NULL, ",");
    species_id = atoi(token);
    token = strtok(NULL, ",");
    height = atoi(token);
    token = strtok(NULL, ",");
    weight = atoi(token);
    token = strtok(NULL, ",");
    base_experience = atoi(token);
    token = strtok(NULL, ",");
    order = atoi(token);
    token = strtok(NULL, "\n");
    is_default = atoi(token);

    p = new Pokemon(id, identifier, species_id, height, weight, base_experience,
                    order, is_default);

    db->pokemon->emplace_back(p);

    return 0;
  }

public:
  CsvParser() { db = new Database(); }
  ~CsvParser() { delete db; }

  int parse(const char *filename) {
    int i;
    ifstream file;
    string path;
    char line[200];

    for (i = 0; !file.is_open(); i++) {
      path = getPath(i, filename);
      cout << path << endl;

      if (path.empty()) {
        cout << "Unable to locate: \"" << filename << ".csv\"" << endl;
        cout << "\tTry one of these: pokemon, moves, pokemon_moves, "
                "pokemon_species, experience, or type_names"
             << endl;
        return 1;
      }

      file.open(path);

      if (file.is_open()) {
        file.getline(line, 200, '\n');
        // cout << "Headers:" << endl;
        // cout << line << endl;
        if (strlen(line) < 1) {
          file.close();
        }
      }
    }

    if (strcmp(filename, "pokemon") == 0) {
      while (file.getline(line, 200, '\n'))
        addPokemon(line);

      db->printPokemon();

    } else if (strcmp(filename, "moves") == 0) {
    } else if (strcmp(filename, "pokemon_moves") == 0) {
    } else if (strcmp(filename, "pokemon_species") == 0) {
    } else if (strcmp(filename, "experience") == 0) {
    } else if (strcmp(filename, "type_names") == 0) {
    }

    file.close();
    return 0;
  }
};

int main(int argc, char *argv[]) {
  CsvParser *parser = new CsvParser();

  parser->parse(argv[1]);

  delete parser;
  return 0;
}
// Files to parse:
//  - pokemon.csv
//  - moves.csv
//  - pokemon_moves.csv
//  - pokemon_species.csv
//  - experience.csv
//  - type_names.csv