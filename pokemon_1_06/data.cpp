#include <fstream>
#include <ios>
#include <iostream>
#include <string>

// File location 1: /share/cs327/pokedex/pokedex/data/csv/
// File location 2: $HOME/cs327/pokedex/pokedex/data/csv/
//      - use getenv() to find the HOME environment variable
// Files to parse:
//  - pokemon.csv
//  - moves.csv
//  - pokemon_moves.csv
//  - pokemon_species.csv
//  - experience.csv
//  - type_names.csv

class Pokemon {
private:
  int id;
  std::string identifier;
  int species_id;
  int height;
  int weight;
  int base_experience;
  int order;
  int is_default;

public:
  Pokemon(int id, std::string identifier, int species_id, int height,
          int weight, int base_experience, int order, int is_default) {
    this->id = id ? id : -1;
    this->identifier = identifier;
    this->species_id = species_id ? species_id : -1;
    this->height = height ? height : -1;
    this->weight = weight ? weight : -1;
    this->base_experience = base_experience ? base_experience : -1;
    this->order = order ? order : -1;
    this->is_default = is_default ? is_default : -1;
  }
};

class Moves {
private:
  int id;
  std::string identifier;
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
  Moves(int id, std::string identifier, int generation_id, int type_id,
        int power, int pp, int accuracy, int priority, int target_id,
        int damage_class_id, int effect_id, int effect_chance,
        int contest_type_id, int contest_effect_id,
        int super_contest_effect_id) {
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
};

class PokemonSpecies {
private:
  int id;
  std::string identifier;
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
  PokemonSpecies(int id, std::string identifier, int generation_id,
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
};

class TypeNames {
private:
  int type_id;
  int local_language;
  std::string name;

public:
  TypeNames(int type_id, int local_language, std::string name) {
    this->type_id = type_id ? type_id : -1;
    this->local_language = local_language ? local_language : -1;
    this->name = name;
  }
};

class CsvParser {
private:
  std::string getPath(int attempt, std::string filename) {
    switch (attempt) {
    case 1:
      return "/share/cs327/pokedex/pokedex/data/csv/" + filename;
    case 2:
      std::string home_dir(getenv("HOME"));
      if (home_dir.empty())
        break;
      return home_dir + "/cs327/pokedex/pokedex/data/csv/" + filename;
    }

    return "";
  }

public:
  CsvParser() {}
  ~CsvParser() {}

  int parse(std::string filename) {
    std::fstream file;
    std::string path;

    for (int i = 0; !file.is_open(); i++) {
      path = getPath(i, filename);

      if (path.empty()) {
        return 1;
      }

      file.open(path, std::ios::in);
    }

    // TODO: parse information

    file.close();
    return 0;
  }
};