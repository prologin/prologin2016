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

let active_cells = Hashtbl.create 200

exception Break

let try_move_vacuum () =
  let f p q =
    match deplacer_aspiration (mkp p) (mkp q) with
    | Ok -> raise Break
    | _ -> ()
  in
  try
    let b = if est_tuyau (mkp (1, tt/2)) then [|0;tt-1|] else [|tt-1;0|] in
    for i = 0 to longueur_base - 1 do
      let x = tt/3 + i in
      if x <> tt/2 && x <> tt/2-1 then
        for j = 0 to 1 do
          let q = (x, b.(j)) in
          f q (tt/2, snd q);
          f q (tt/2-1, snd q);
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

let downstream ((x, y) as p') =
  neighbors p'
    |> List.filter
      (fun (x1, y1) -> None <> ds.(x1).(y1) && ds.(x).(y) > ds.(x1).(y1))

let rec mark tgt ((x, y) as p') =
  match path_to.(x).(y) with
  | tgt' :: _ when tgt = tgt' -> ()
  | tgts ->
      path_to.(x).(y) <- tgt :: tgts;
      downstream p' |> List.iter (mark tgt)

let _ = liste_pulsars () |> Array.to_list |> List.map unp >>= neighbors |> List.iter (fun p' -> mark p' p')

let path_to = Array.map (Array.map List.length) path_to

let to_build = ref None

let rec maximum_fst = function
  | [x] -> x
  | ((x1, _) as q) :: xs ->
      let (x2, _) as p = maximum_fst xs in
      if x1 < x2 then p else q
  | _ -> assert false

let downstream' p' k =
  match downstream p' with
  | [] -> ()
  | xs -> xs |> List.map (fun ((x1, y1) as p1) -> path_to.(x1).(y1), p1)
    |> maximum_fst |> snd |> k

let construire' p =
  match construire p with
  | Ok -> Hashtbl.add active_cells p ()
  | _ -> ()

let build_from { periode ; pulsations_restantes } ((x, y) as p') =
  let n = ref 0 in
  let b = ref (fun () -> ()) in
  let rec build_from ((x, y) as p') =
    downstream' p' build_from;
    let b0 = !b in
    if est_libre (mkp p') then (
      n := !n + cout_construction;
      b := fun () -> b0 (); ignore (construire' (mkp p'))
    ) else if est_debris (mkp p') then (
      n := !n + cout_deblayage + cout_construction;
      b := fun () -> b0 (); ignore (deblayer (mkp p')); ignore (construire' (mkp p'));
    );
  in
  match ds.(x).(y) with
  | None -> ()
  | Some d ->
      build_from p';
      let tour_fin = 2 * (!n / nb_points_action) + tour_actuel () in
      let tour_epuise =
        let x = tour_actuel () + periode * pulsations_restantes in
        x - (x mod periode)
      in
      let w = ((tour_fin - 1) / periode + 1) * periode in
      match !to_build with
      | _ when !n = 0 || pulsations_restantes = 0 || tour_epuise < tour_fin -> ()
      | Some (v, c) when v < w -> ()
      | _ -> to_build := Some (w, !b)

let try_fix p () =
  if est_debris p then ignore (deblayer p);
  if est_libre p then ignore (construire p)

let try_fix_pipes () =
  Hashtbl.iter try_fix active_cells

let try_build_pipes () =
  let x = ref (-1) in
  while points_action () <> !x do
    x := points_action ();
    to_build := None;
    liste_pulsars ()
      |> Array.iter (fun p ->
        let info = info_pulsar p in
        neighbors (unp p) |> List.iter (build_from info));
    match !to_build with
    | None -> ()
    | Some (_, b) -> b ()
  done

let aspi = Array.make_matrix tt tt (max_int, [], [])

let reset_aspi () =
  for i = 1 to tt - 2 do
    for j = 1 to tt - 2 do
      aspi.(i).(j) <- (max_int, [], [])
    done
  done

let go p0 p =
  let k = puissance_aspiration (mkp p0) in
  let q = Queue.create () in
  Queue.push (-k, p) q;
  while not (Queue.is_empty q) do
    let d, ((x, y) as p) = Queue.pop q in
    match aspi.(x).(y) with
    | _, p1 :: _, _ | _, _, p1 :: _ when p0 = p1 -> ()
    | e, s, r when est_tuyau (mkp p) ->
        if e = d then aspi.(x).(y) <- (e, p0 :: s, r)
        else if d < e then aspi.(x).(y) <- (d, [p0], s @ r)
        else aspi.(x).(y) <- (d, s, p0 :: r);
        neighbors p |> List.iter (fun p -> Queue.push (d+1, p) q)
    | _ -> ()
  done

let update_aspi () =
  reset_aspi ();
  for i = tt/3 to tt/3 + longueur_base - 1 do
    go (i,0) (i,1);
    go (i,tt-1) (i,tt-2);
    go (0,i) (1,i);
    go (tt-1,i) (tt-2,i);
  done

let destroy' (x, y) =
  if x = 0 then
    (ignore (detruire (mkp (1, y))); raise Break)
  else if x = tt-1 then
    (ignore (detruire (mkp (tt-2, y))); raise Break)

let try_destroy () =
  let worth w p =
    charges_presentes (mkp p) > w *. charge_destruction in
  for i = 1 to tt-2 do
    [(2, i); (3, i); (tt-3, i); (tt-4, i)]
      |> List.iter (fun ((x, y) as p) ->
        match aspi.(x).(y) with
        | _, [p0], [] when worth 1. p ->
            destroy' p0
        | _ -> ())
  done;
  for i = 1 to tt-2 do
    for j = 1 to tt-2 do
      let _, aa, _ = aspi.(i).(j) in
      let f (x, _) = x = 0 || x = tt-1 in
      match aa, List.filter f aa with
      | _ :: _ :: _, [p0] when worth 2. (i, j) -> destroy' p0
      | _ -> ()
    done
  done

let try_destroy () = try try_destroy () with Break -> ()

let first_path = ref true

let try_double_pipe () =
  if tour_actuel () > 8 then
    for i = 1 to tt-1 do
      if (i < tt/3 || i > (2 * tt)/3)
        && est_tuyau (mkp (tt/2, i)) then
        construire' (mkp (tt/2-1, i))
    done

let partie_init () = ()

let jouer_tour () =
  update_aspi ();
  try_move_vacuum ();
  try_double_pipe ();
  try_fix_pipes ();
  try_destroy ();
  try_build_pipes ()

let partie_fin () = ();;

(* /!\ Ne touche pas a ce qui suit /!\ *)
Callback.register "ml_partie_init" partie_init;;Callback.register "ml_jouer_tour" jouer_tour;;Callback.register "ml_partie_fin" partie_fin;;
