#include <bits/stdc++.h>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <list>
#include <new>
#include <ostream>
#include <sstream>
#include <string>

using namespace std;

#define stoi(a) (a.empty() ? -1 : stoi(a))

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
    if (p.id != -1)
      os << p.id;
    os << ",";
    os << p.identifier;
    os << ",";
    if (p.species_id != -1)
      os << p.species_id;
    os << ",";
    if (p.height != -1)
      os << p.height;
    os << ",";
    if (p.weight != -1)
      os << p.weight;
    os << ",";
    if (p.base_experience != -1)
      os << p.base_experience;
    os << ",";
    if (p.order != -1)
      os << p.order;
    os << ",";
    if (p.is_default != -1)
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

  friend std::ostream &operator<<(std::ostream &os, const Moves &m) {
    if (m.id != -1)
      os << m.id;
    os << ",";
    os << m.identifier;
    os << ",";
    if (m.generation_id != -1)
      os << m.generation_id;
    os << ",";
    if (m.type_id != -1)
      os << m.type_id;
    os << ",";
    if (m.power != -1)
      os << m.power;
    os << ",";
    if (m.pp != -1)
      os << m.pp;
    os << ",";
    if (m.accuracy != -1)
      os << m.accuracy;
    os << ",";
    if (m.priority != -1)
      os << m.priority;
    os << ",";
    if (m.target_id != -1)
      os << m.target_id;
    os << ",";
    if (m.damage_class_id != -1)
      os << m.damage_class_id;
    os << ",";
    if (m.effect_id != -1)
      os << m.effect_id;
    os << ",";
    if (m.effect_chance != -1)
      os << m.effect_chance;
    os << ",";
    if (m.contest_type_id != -1)
      os << m.contest_type_id;
    os << ",";
    if (m.contest_effect_id != -1)
      os << m.contest_effect_id;
    os << ",";
    if (m.super_contest_effect_id != -1)
      os << m.super_contest_effect_id;
    return os;
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

  friend std::ostream &operator<<(std::ostream &os, const PokemonMoves &pm) {
    if (pm.pokemon_id != -1)
      os << pm.pokemon_id;
    os << ",";
    if (pm.version_group_id != -1)
      os << pm.version_group_id;
    os << ",";
    if (pm.move_id != -1)
      os << pm.move_id;
    os << ",";
    if (pm.pokemon_move_method_id != -1)
      os << pm.pokemon_move_method_id;
    os << ",";
    if (pm.level != -1)
      os << pm.level;
    os << ",";
    if (pm.order != -1)
      os << pm.order;
    return os;
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

  friend std::ostream &operator<<(std::ostream &os, const PokemonSpecies &ps) {
    if (ps.id != -1)
      os << ps.id;
    os << ",";
    os << ps.identifier;
    os << ",";
    if (ps.generation_id != -1)
      os << ps.generation_id;
    os << ",";
    if (ps.evolves_from_species_id != -1)
      os << ps.evolves_from_species_id;
    os << ",";
    if (ps.evolution_chain_id != -1)
      os << ps.evolution_chain_id;
    os << ",";
    if (ps.color_id != -1)
      os << ps.color_id;
    os << ",";
    if (ps.shape_id != -1)
      os << ps.shape_id;
    os << ",";
    if (ps.habitat_id != -1)
      os << ps.habitat_id;
    os << ",";
    if (ps.gender_rate != -1)
      os << ps.gender_rate;
    os << ",";
    if (ps.capture_rate != -1)
      os << ps.capture_rate;
    os << ",";
    if (ps.base_happiness != -1)
      os << ps.base_happiness;
    os << ",";
    if (ps.is_baby != -1)
      os << ps.is_baby;
    os << ",";
    if (ps.hatch_counter != -1)
      os << ps.hatch_counter;
    os << ",";
    if (ps.has_gender_differences != -1)
      os << ps.has_gender_differences;
    os << ",";
    if (ps.growth_rate_id != -1)
      os << ps.growth_rate_id;
    os << ",";
    if (ps.forms_switchable != -1)
      os << ps.forms_switchable;
    os << ",";
    if (ps.is_legendary != -1)
      os << ps.is_legendary;
    os << ",";
    if (ps.is_mythical != -1)
      os << ps.is_mythical;
    os << ",";
    if (ps.order != -1)
      os << ps.order;
    os << ",";
    if (ps.conquest_order != -1)
      os << ps.conquest_order;
    return os;
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

  friend std::ostream &operator<<(std::ostream &os, const Experience &e) {
    if (e.growth_rate_id != -1)
      os << e.growth_rate_id;
    os << ",";
    if (e.level != -1)
      os << e.level;
    os << ",";
    if (e.experience != -1)
      os << e.experience;
    return os;
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

  friend std::ostream &operator<<(std::ostream &os, const TypeNames &tn) {
    if (tn.type_id != -1)
      os << tn.type_id;
    os << ",";
    if (tn.local_language != -1)
      os << tn.local_language;
    os << ",";
    os << tn.name;
    return os;
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

  void printMoves() {
    list<Moves>::iterator itr;
    for (itr = moves->begin(); itr != moves->end(); ++itr)
      cout << *itr << endl;
  }

  void printPokemonMoves() {
    list<PokemonMoves>::iterator itr;
    for (itr = pokemonMoves->begin(); itr != pokemonMoves->end(); ++itr)
      cout << *itr << endl;
  }

  void printPokemonSpecies() {
    list<PokemonSpecies>::iterator itr;
    for (itr = pokemonSpecies->begin(); itr != pokemonSpecies->end(); ++itr)
      cout << *itr << endl;
  }

  void printExperience() {
    list<Experience>::iterator itr;
    for (itr = experience->begin(); itr != experience->end(); ++itr)
      cout << *itr << endl;
  }

  void printTypeNames() {
    list<TypeNames>::iterator itr;
    for (itr = typeNames->begin(); itr != typeNames->end(); ++itr)
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
    Pokemon *p;
    int id;
    string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    id = stoi(tokens[0]);
    identifier = tokens[1];
    species_id = stoi(tokens[2]);
    height = stoi(tokens[3]);
    weight = stoi(tokens[4]);
    base_experience = stoi(tokens[5]);
    order = stoi(tokens[6]);
    is_default = stoi(tokens[7]);

    p = new Pokemon(id, identifier, species_id, height, weight, base_experience,
                    order, is_default);

    db->pokemon->emplace_back(p);

    return 0;
  }

  int addMoves(char *str) {
    Moves *m;

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

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    id = stoi(tokens[0]);
    identifier = tokens[1];
    generation_id = stoi(tokens[2]);
    type_id = stoi(tokens[3]);
    power = stoi(tokens[4]);
    pp = stoi(tokens[5]);
    accuracy = stoi(tokens[6]);
    priority = stoi(tokens[7]);
    target_id = stoi(tokens[8]);
    damage_class_id = stoi(tokens[9]);
    effect_id = stoi(tokens[10]);
    effect_chance = stoi(tokens[11]);
    contest_type_id = stoi(tokens[12]);
    contest_effect_id = stoi(tokens[13]);
    super_contest_effect_id = stoi(tokens[14]);

    m = new Moves(id, identifier, generation_id, type_id, power, pp, accuracy,
                  priority, target_id, damage_class_id, effect_id,
                  effect_chance, contest_type_id, contest_effect_id,
                  super_contest_effect_id);

    db->moves->emplace_back(m);

    return 0;
  }

  int addPokemonMoves(char *str) {
    PokemonMoves *pm;

    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    pokemon_id = stoi(tokens[0]);
    version_group_id = stoi(tokens[1]);
    move_id = stoi(tokens[2]);
    pokemon_move_method_id = stoi(tokens[3]);
    level = stoi(tokens[4]);
    order = stoi(tokens[5]);

    pm = new PokemonMoves(pokemon_id, version_group_id, move_id,
                          pokemon_move_method_id, level, order);

    db->pokemonMoves->emplace_back(pm);

    return 0;
  }

  int addPokemonSpecies(char *str) {
    PokemonSpecies *ps;

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

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    id = stoi(tokens[0]);
    identifier = tokens[1];
    generation_id = stoi(tokens[2]);
    evolves_from_species_id = stoi(tokens[3]);
    evolution_chain_id = stoi(tokens[4]);
    color_id = stoi(tokens[5]);
    shape_id = stoi(tokens[6]);
    habitat_id = stoi(tokens[7]);
    gender_rate = stoi(tokens[8]);
    capture_rate = stoi(tokens[9]);
    base_happiness = stoi(tokens[10]);
    is_baby = stoi(tokens[11]);
    hatch_counter = stoi(tokens[12]);
    has_gender_differences = stoi(tokens[13]);
    growth_rate_id = stoi(tokens[14]);
    forms_switchable = stoi(tokens[15]);
    is_legendary = stoi(tokens[16]);
    is_mythical = stoi(tokens[17]);
    order = stoi(tokens[18]);
    conquest_order = stoi(tokens[19]);

    ps = new PokemonSpecies(
        id, identifier, generation_id, evolves_from_species_id,
        evolution_chain_id, color_id, shape_id, habitat_id, gender_rate,
        capture_rate, base_happiness, is_baby, hatch_counter,
        has_gender_differences, growth_rate_id, forms_switchable, is_legendary,
        is_mythical, order, conquest_order);

    db->pokemonSpecies->emplace_back(ps);

    return 0;
  }

  int addExperience(char *str) {
    Experience *e;

    int growth_rate_id;
    int level;
    int experience;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    growth_rate_id = stoi(tokens[0]);
    level = stoi(tokens[1]);
    experience = stoi(tokens[2]);

    e = new Experience(growth_rate_id, level, experience);

    db->experience->emplace_back(e);

    return 0;
  }

  int addTypeNames(char *str) {
    TypeNames *tn;

    int type_id;
    int local_language;
    string name;

    vector<string> tokens;
    stringstream check(str);
    string tmp;
    while (getline(check, tmp, ','))
      tokens.push_back(tmp);

    type_id = stoi(tokens[0]);
    local_language = stoi(tokens[1]);
    name = tokens[2];

    tn = new TypeNames(type_id, local_language, name);

    db->typeNames->emplace_back(tn);

    return 0;
  }

public:
  CsvParser() { db = new Database(); }
  ~CsvParser() { delete db; }

  int parse(const char *filename) {
    int i;
    ifstream file;
    string path;
    char line[400];

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
        file.getline(line, 400, '\n');
        // cout << "Headers:" << endl;
        if (strlen(line) < 1) {
          file.close();
        }
      }
    }

    if (strcmp(filename, "pokemon") == 0) {
      while (file.getline(line, 400, '\n'))
        addPokemon(line);
      db->printPokemon();

    } else if (strcmp(filename, "moves") == 0) {
      while (file.getline(line, 400, '\n'))
        addMoves(line);
      db->printMoves();

    } else if (strcmp(filename, "pokemon_moves") == 0) {
      while (file.getline(line, 400, '\n'))
        addPokemonMoves(line);
      db->printPokemonMoves();

    } else if (strcmp(filename, "pokemon_species") == 0) {
      while (file.getline(line, 400, '\n'))
        addPokemonSpecies(line);
      db->printPokemonSpecies();

    } else if (strcmp(filename, "experience") == 0) {
      while (file.getline(line, 400, '\n'))
        addExperience(line);
      db->printExperience();
    } else if (strcmp(filename, "type_names") == 0) {
      while (file.getline(line, 400, '\n'))
        addTypeNames(line);
      db->printTypeNames();
    } else {
      cout << "\tTry one of these: pokemon, moves, pokemon_moves, "
              "pokemon_species, experience, or type_names"
           << endl;
    }

    file.close();
    return 0;
  }
};

// int main(int argc, char *argv[]) {
//   CsvParser *parser = new CsvParser();

//   parser->parse(argv[1]);

//   delete parser;
//   return 0;
// }
// Files to parse:
//  - pokemon.csv
//  - moves.csv
//  - pokemon_moves.csv
//  - pokemon_species.csv
//  - experience.csv
//  - type_names.csv