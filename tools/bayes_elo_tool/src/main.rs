use bayes_elo::BayesElo;
use std::env;
use std::io;
use std::error::Error;

fn main() -> Result<(), Box<dyn Error>>{
    let mut v_elo: Vec<f64> = Vec::new();
    let mut i = 0;
    for arg in env::args() {
        println!("{} - {}", i, arg);

        if i > 0 {
            let num = arg.parse::<f64>();

            match num {
                Ok(n) => {
                    println!("parse elo ok: {}", n);
                    v_elo.push(n);
                }
                Err(e) => panic!("parse elo error: {}", e),
            }
        }

        i += 1;
    }
    let mut first_player_elo = 0.0;
    let mut second_player_elo = 0.0;
    println!("init elo: {}, {}", first_player_elo, second_player_elo);
    if 2 == v_elo.len() {
        first_player_elo = v_elo[0];
        second_player_elo = v_elo[1];
    } else {
        println!(
            "{:?} [first_player_elo] [second_player_elo]",
            env::current_exe()?
        );
        panic!("Parameters' length error!!!");
    }

    let mut bayes_elo_instance = BayesElo::new();
    loop {
        println!("**************************new game**************************");
        println!("old elo: {}, {}", first_player_elo, second_player_elo);
        bayes_elo_instance.set_k_factor(6000.0 / (first_player_elo + second_player_elo));

        let mut input_1 = String::new();
        println!("If exchange player? (y/n): ");
        io::stdin().read_line(&mut input_1)?;
        let mut v_str_1: Vec<&str> = Vec::new();
        for str_value in input_1.split_whitespace() {
            v_str_1.push(str_value);
        }
        if 1 == v_str_1.len() {
            if "y" == v_str_1[0] {
                (first_player_elo, second_player_elo) = (second_player_elo, first_player_elo);
                println!("new elo: {}, {}", first_player_elo, second_player_elo);
            } else if "n" == v_str_1[0] {
            } else {
            }
        }

        let mut input_2 = String::new();
        println!("If first player win? (y/n/d): ");
        io::stdin().read_line(&mut input_2)?;
        let mut v_str_2: Vec<&str> = Vec::new();
        for str_value in input_2.split_whitespace() {
            v_str_2.push(str_value);
        }
        if 1 == v_str_2.len() {
            if v_str_2[0] == "y" {
                (first_player_elo, second_player_elo) =
                    bayes_elo_instance.calculate(first_player_elo, second_player_elo, true);
            } else if v_str_2[0] == "n" {
                (second_player_elo, first_player_elo) =
                    bayes_elo_instance.calculate(second_player_elo, first_player_elo, false);
            } else {
                (first_player_elo, second_player_elo) =
                    bayes_elo_instance.calculate_4_draw(first_player_elo, second_player_elo, true);
            }
            println!("new elo: {}, {}", first_player_elo, second_player_elo);
        } else {
            panic!("Parameters error!!!");
        }

        println!("**************************end game**************************");
        let mut input_3 = String::new();
        println!("If exit program? (y/n): ");
        io::stdin().read_line(&mut input_3)?;
        let mut v_str_3: Vec<&str> = Vec::new();
        for str_value in input_3.split_whitespace() {
            v_str_3.push(str_value);
        }
        if 1 == v_str_3.len() {
            if "y" == v_str_3[0] {
                break;
            } else {

            }
        } else if 0 == v_str_3.len() {
            println!("Go on!");
        } else {
            panic!("Parameters error!!!");
        }
    }
    Ok(())
}
