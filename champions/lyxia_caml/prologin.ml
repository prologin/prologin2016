open Api;;

(* Use only one base cell on each side. Try to connect all cells next to
 * pulsars to base using few pipes overall. No destruction.
 *)

let swapping = snd (ma_base ()).(0) = 0

let unp = if swapping then fun (x, y) -> x, y else fun (x, y) -> y, x
let mkp = unp

let tt = taille_terrain

let (>>=) x f = x |> List.map f |> List.concat

let neighbors (x, y) = [(x+1, y); (x, y+1); (x, y-1); (x-1, y)]

exception Break

let try_move_vacuum () =
  try
    for i = 0 to longueur_base - 1 do
      for j = 0 to 1 do
        let q = (tt/3 + i, [|0; tt-1|].(j)) in
        match deplacer_aspiration (mkp q) (mkp (tt / 2, snd q)) with
        | Ok -> raise Break
        | _ -> ()
      done
    done
  with
  | Break -> ()

let ds = Array.make_matrix tt tt None

let _ =
  let q = Queue.create () in
  List.iter
    (fun ((x, y) as p') ->
      ds.(x).(y) <- Some 0;
      Queue.push (p', 0) q)
    [(tt/2, 1); (tt/2, tt-2)];
  while not (Queue.is_empty q) do
    let (p', d) = Queue.pop q in
    neighbors p' |> List.iter (fun ((x, y) as p') ->
      match ds.(x).(y), type_case (mkp p') with
      | None, Vide ->
          ds.(x).(y) <- Some (d+1);
          Queue.push (p', d+1) q
      | _ -> ());
  done

let path_to = Array.make_matrix tt tt []

module PSet = Set.Make (struct type t = int * int let compare = compare end)

let pulsar_neighbors = liste_pulsars () |> Array.to_list |> List.map unp >>= neighbors |> PSet.of_list

let rec mark tgt ((x, y) as p') =
  match path_to.(x).(y) with
  | tgt' :: _ when tgt = tgt' -> ()
  | tgts ->
      path_to.(x).(y) <- tgt :: tgts;
      try
        neighbors p'
          |> List.filter
            (fun (x1, y1) -> None <> ds.(x1).(y1) && ds.(x).(y) > ds.(x1).(y1))
          |> List.iter (mark tgt)
      with
      | Not_found -> ()

let _ = pulsar_neighbors |> PSet.iter (fun p' -> mark p' p')

let pipes_to_build = ref []

let rec maximum_fst = function
  | [x] -> x
  | ((x1, _) as q) :: xs ->
      let (x2, _) as p = maximum_fst xs in
      if x1 < x2 then p else q
  | _ -> assert false

let rec flow ((x, y) as p) =
  let go ps =
    ps
      |> List.map (fun ((x1, y1) as p1) ->
        (List.length path_to.(x1).(y1), p1))
      |> maximum_fst
      |> snd
      |> flow
  in
  pipes_to_build := p :: !pipes_to_build;
  neighbors p
    |> List.filter (fun (x1, y1) ->
        None <> ds.(x1).(y1) && ds.(x).(y) > ds.(x1).(y1))
    |> function
      | [] -> ()
      | ps -> go ps

let _ = pulsar_neighbors |> PSet.iter flow

let pipes_to_build = !pipes_to_build

let try_fix p =
  match type_case (mkp p) with
  | Debris ->
      deblayer (mkp p);
      raise Break
  | _ -> ()

let try_fix_pipes () =
  try
    List.iter try_fix pipes_to_build
  with
  | Break -> ()

let try_build p =
  match construire (mkp p) with
  | Ok -> raise Break
  | _ -> ()

let try_build_pipes () =
  while points_action () >= cout_construction_tuyau &&
    try
      List.iter try_build pipes_to_build;
      false
    with
    | Break -> true
  do () done

let partie_init () = ()

let jouer_tour () =
  try_move_vacuum ();
  try_fix_pipes ();
  try_build_pipes ()

let partie_fin () = ();;

(* /!\ Ne touche pas a ce qui suit /!\ *)
Callback.register "ml_partie_init" partie_init;;Callback.register "ml_jouer_tour" jouer_tour;;Callback.register "ml_partie_fin" partie_fin;;
