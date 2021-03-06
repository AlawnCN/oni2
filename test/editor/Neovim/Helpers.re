open Oni_Core;
open Oni_Neovim;

let getNeovimPath = () => {
  let {neovimPath, _}: Setup.t = Setup.init();
  neovimPath;
};

let repeat = Oni_Core_Test.Helpers.repeat;

let uiAttach = (api: NeovimApi.t) => {
  api.requestSync(
    "nvim_ui_attach",
    Msgpck.List([
      Msgpck.Int(20),
      Msgpck.Int(20),
      Msgpck.Map([
        (Msgpck.String("rgb"), Msgpck.Bool(true)),
        (Msgpck.String("ext_popupmenu"), Msgpck.Bool(true)),
        (Msgpck.String("ext_tabline"), Msgpck.Bool(true)),
        (Msgpck.String("ext_cmdline"), Msgpck.Bool(true)),
        (Msgpck.String("ext_wildmenu"), Msgpck.Bool(true)),
        (Msgpck.String("ext_linegrid"), Msgpck.Bool(true)),
        /* (Msgpck.String("ext_multigrid"), Msgpck.Bool(true)), */
        /* (Msgpck.String("ext_hlstate"), Msgpck.Bool(true)), */
      ]),
    ]),
  );
};

let withNeovimApi = f => {
  let {neovimPath, _}: Setup.t = Setup.init();
  let nvim =
    NeovimProcess.start(
      ~neovimPath,
      ~args=[|"-u", "NORC", "--noplugin", "--embed"|],
    );
  let msgpackTransport =
    MsgpackTransport.make(
      ~onData=nvim.stdout.onData,
      ~write=nvim.stdin.write,
      (),
    );
  let nvimApi = NeovimApi.make(msgpackTransport);

  f(nvimApi);

  msgpackTransport.close();
};

let withNeovimProtocol = f => {
  withNeovimApi(nvimApi => {
    let neovimProtocol = NeovimProtocol.make(nvimApi);
    let _ = uiAttach(nvimApi);
    f((nvimApi, neovimProtocol));
  });
};
