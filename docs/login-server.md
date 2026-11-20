# Separate login server and shard selection

UOX3 supports the classic Ultima Online login flow: authenticate once, choose a shard, receive a relay packet, and reconnect to the selected game server with a one-time ticket.

Existing installations remain unchanged because `SERVERTYPE` defaults to `COMBINED`.

## Roles

- `COMBINED` runs the traditional login and game server together.
- `LOGIN` loads only networking and the central account database. It does not load maps, scripts, regions, or world saves.
- `GAME` runs a shard and accepts game logins only after the login server registers a signed handoff ticket.

Use a separate working directory and separate `uox.ini` for the login server and every game shard. In particular, do not share `accounts.adm`, `shared`, world-save, or log directories between processes. The login server owns normal player credentials. Staff credentials can remain only on their game shard by using the direct administrator login described below. Each game shard automatically creates its own local account record on the first successful central handoff so character ownership remains shard-local.

## Login server configuration

Add these settings to the login server's `[system]` section:

```ini
SERVERTYPE=LOGIN
SERVERNAME=UOX Login
PORT=2593
SHARDLIST=shards.ini
LOGINHANDOFFSECRET=replace-with-the-same-long-random-secret
LOGINHANDOFFTIMEOUT=30
```

The central account settings, including `INTERNALACCOUNTCREATION`, `PASSWORDHASHINGENABLED`, and login throttling, continue to work normally.

The shard registry is a separate file so rewriting `uox.ini` cannot lose repeated shard entries:

```ini
[shards]
{
SHARD=1|Britannia|203.0.113.10|2594|10.0.0.10
SHARD=2|Ilshenar|203.0.113.11|2595|10.0.0.11
}
```

Each line is `SHARD=id|display name|advertised IPv4|game port|handoff host`.

- `id` must be unique and nonzero.
- `display name` is limited to 32 characters by the UO protocol.
- `advertised IPv4` is the public or LAN IPv4 address sent to the client. Hostnames and IPv6 cannot be encoded in the legacy relay packet.
- `handoff host` is optional and defaults to the advertised address. It can be a DNS name or private address reachable by the login server.

See `data/shards.ini.example` for a ready-to-copy template.

## Game shard configuration

Each game shard uses the same secret and its matching ID:

```ini
SERVERTYPE=GAME
SHARDID=1
SERVERNAME=Britannia
PORT=2594
LOGINHANDOFFSECRET=replace-with-the-same-long-random-secret
LOGINHANDOFFTIMEOUT=30
ALLOWLOCALADMINLOGIN=0
LOCALADMINIPS=127.0.0.1
```

Start the game shards first, then the login server. Point the UO client at the login server's address and port.

The login server registers each ticket with the selected shard over that shard's normal game port before sending the client a relay. The registration is authenticated with HMAC-SHA256. Tickets expire quickly, are bound to the account and client IP, and can be consumed only once. The legacy client still includes its password field in packet `0x91`, but the game shard ignores it, never validates it, and stores only a random shard-local password; the login server does not copy central credentials to a shard.

## Shard-local administrator login

A game shard can optionally let its GM owner authenticate directly without placing that administrator's credentials on the central login server:

```ini
SERVERTYPE=GAME
ALLOWLOCALADMINLOGIN=1
LOCALADMINIPS=198.51.100.25
```

`LOCALADMINIPS` is a comma-separated list of exact IPv4 source addresses. It must contain the administrator's static public IP, not the shard's own public IP. Use `127.0.0.1` only when the UO client runs on the shard machine. The shard's public address remains configured through `EXTERNALIP`.

Direct login is accepted only when the source IP is allowlisted and the username/password belong to a shard-local account with the GM flag. Unknown and non-GM accounts cannot be created or authenticated through this route. The client connects directly to the game shard's normal address and port; the shard displays itself as the only shard and completes the connection with a short-lived, one-use local ticket.

When local administrator login is enabled, central handoffs are refused for every locally privileged GM, seer, or counselor account. Central tickets transfer only ordinary public/young status and cannot grant staff privileges. Use a unique password for each shard-local administrator, enable password hashing, and also restrict the administrator's static IP in the host firewall when possible.

Keep clocks synchronized on all machines, use a different random secret in production, and restrict each game port with normal firewall rules. If a selected shard is unavailable or rejects the handoff, the login server denies the relay instead of sending the client to a broken destination.

## Complete three-folder setup

The simplest layout for one login server and two shards is:

```text
UOX3 Servers\
|-- Login\
|   |-- accounts\
|   |   `-- accounts.adm
|   |-- logs\
|   |-- shards.ini
|   |-- uox.ini
|   `-- uox3.exe
|-- Britannia\
|   |-- accounts\
|   |-- archives\
|   |-- books\
|   |-- dfnData\
|   |-- dictionaries\
|   |-- js\
|   |-- logs\
|   |-- msgboards\
|   |-- shared\
|   |-- uox.ini
|   `-- uox3.exe
`-- TestShard\
    |-- accounts\
    |-- archives\
    |-- books\
    |-- dfnData\
    |-- dictionaries\
    |-- js\
    |-- logs\
    |-- msgboards\
    |-- shared\
    |-- uox.ini
    `-- uox3.exe
```

Every game shard is a complete UOX3 installation with its own data, scripts, saves, accounts, and logs. Do not point two running shards at the same folders. The login folder is deliberately small because a `LOGIN` process does not load the game world, maps, DFNs, or JavaScript. Keep an `accounts` and `logs` directory in it, even when they are initially empty.

### Converting an existing single-shard installation

1. Stop UOX3 and make a backup of the entire existing installation.
2. Make a `Britannia` folder and copy the complete existing installation into it. This preserves its world, characters, and shard-local administrator account.
3. Create `TestShard` from a separate clean UOX3 installation if it should have a new world. Copying `Britannia` also copies its world and accounts, so only do that when a deliberate clone is wanted.
4. Create the small `Login` folder shown above. Copy the current `uox3.exe` into it, create its own `accounts` and `logs` folders, and give it its own `uox.ini` and `shards.ini`.
5. Put normal player login accounts in `Login\accounts\accounts.adm`. Remove staff accounts from this central copy. Do not remove the administrator from the game shard's account database.
6. Give the three processes different ports and the two shards different nonzero shard IDs.

Do not run one executable from a parent directory and expect it to find another folder's INI. Start each `uox3.exe` with its working directory set to the folder containing that executable and its `uox.ini`.

A central handoff never copies a player's password to a game shard; newly provisioned shard accounts receive a random local password. However, a shard cloned from an old combined installation can still contain its old player passwords because those records already existed before the split. Normal player handoffs do not use those old local passwords. If strict separation of all historical player credentials is required, plan a separate credential migration and keep pre-split backups away from the login-server operator.

## Address names explained

Three different addresses appear in this setup:

- The login address is where normal players initially connect, normally port `2593`.
- The advertised shard address in `shards.ini` is sent to the UO client after shard selection. It must be the public or LAN IPv4 address that the player can reach.
- `LOCALADMINIPS` contains the administrator's source IP as seen by the game shard. It is not the shard's address.

For example, if Britannia is hosted at `203.0.113.40` and the owner connects from a home connection whose static public IP is `198.51.100.25`, use the shard address for `EXTERNALIP` and the owner's address for `LOCALADMINIPS`:

```ini
EXTERNALIP=203.0.113.40
LOCALADMINIPS=198.51.100.25
```

When every process and client runs on one PC, use `127.0.0.1`. On a LAN, use the server's LAN IP in the advertised field and the admin computer's LAN IP in `LOCALADMINIPS`. Across the internet, use public static addresses. If NAT is present, forward each port to the appropriate server and enter the public source address the shard actually sees.

## Complete configuration examples

These are the settings that differ between the three folders. Keep the rest of each installation's normal `uox.ini` settings.

### `Login\uox.ini`

Put these lines inside the `[system]` section:

```ini
SERVERNAME=UOX Login
SERVERTYPE=LOGIN
SHARDID=0
SHARDLIST=shards.ini
LOGINHANDOFFSECRET=use-the-same-long-random-secret-on-all-three-servers
LOGINHANDOFFTIMEOUT=30
EXTERNALIP=127.0.0.1
PORT=2593
ALLOWLOCALADMINLOGIN=0
LOCALADMINIPS=127.0.0.1
```

`ALLOWLOCALADMINLOGIN` must remain off on the login server. Account creation and password hashing settings in this INI apply to central player accounts.

### `Login\shards.ini`

For a same-PC test:

```ini
// Format: SHARD=id|display name|advertised IPv4|game port|handoff host
[shards]
{
SHARD=1|Britannia|127.0.0.1|2594|127.0.0.1
SHARD=2|Test Shard|127.0.0.1|2595|127.0.0.1
}
```

For separate hosts, replace each advertised address with that shard's player-reachable static IPv4. The optional final handoff host is the address or DNS name the login server itself uses to contact that shard. It may be a private network address when the machines share a private network.

### `Britannia\uox.ini`

```ini
SERVERNAME=Britannia
SERVERTYPE=GAME
SHARDID=1
SHARDLIST=
LOGINHANDOFFSECRET=use-the-same-long-random-secret-on-all-three-servers
LOGINHANDOFFTIMEOUT=30
EXTERNALIP=127.0.0.1
PORT=2594
ALLOWLOCALADMINLOGIN=1
LOCALADMINIPS=127.0.0.1
```

### `TestShard\uox.ini`

```ini
SERVERNAME=Test Shard
SERVERTYPE=GAME
SHARDID=2
SHARDLIST=
LOGINHANDOFFSECRET=use-the-same-long-random-secret-on-all-three-servers
LOGINHANDOFFTIMEOUT=30
EXTERNALIP=127.0.0.1
PORT=2595
ALLOWLOCALADMINLOGIN=1
LOCALADMINIPS=127.0.0.1
```

Use the same handoff secret on the login server and both shards. Use a long, randomly generated secret and never publish it. Each shard ID and port must be unique. The advertised address and port in `shards.ini` must match the game shard that owns that ID.

## Keeping administrator accounts off the login server

The staff account remains in each shard's own `accounts\accounts.adm` and per-account directory. It should not exist in `Login\accounts\accounts.adm`, `Login\accounts\newaccounts.adm`, or a per-account folder under `Login\accounts`.

For a direct administrator connection:

1. Set `ALLOWLOCALADMINLOGIN=1` on the game shard.
2. Put the administrator's exact static source IPv4 in `LOCALADMINIPS`. Multiple exact addresses can be separated with commas.
3. Confirm the account already exists locally on that shard and has the GM flag.
4. Configure the UO client to connect directly to that shard's static address and game port, such as `203.0.113.40:2594`.
5. Log in with the shard-local administrator username and password. The shard shows itself as the only available shard and completes the normal relay sequence locally.

All three checks are required: direct login must be enabled, the source IP must match, and the local account must be a GM. A correct password by itself is not enough. Unknown accounts and ordinary player accounts cannot use direct login. If a shard owner works from a new public IP, update `LOCALADMINIPS` locally before that address can connect.

Use a unique password for each shard's administrator. Enabling `PASSWORDHASHINGENABLED=1` is strongly recommended after following the normal password migration procedure. Also allow the admin source IP in the host or cloud firewall; application checks and firewall rules should both be used.

### Security boundary

The central login server never receives or stores the direct shard administrator password. A central handoff cannot grant GM, seer, or counselor flags, even if its signed ticket claims those flags. When local administrator login is enabled, a central handoff also cannot enter an existing locally privileged account with the same name.

The central login server is still trusted for ordinary player authentication. Its operator can control where ordinary players are relayed, so use only a trusted login host and protect the handoff secret. An administrator should connect directly to the shard and should never type the shard-local administrator password into the central login server.

## Local and global account bans

Split login mode has two deliberately separate ban scopes:

- A local ban is stored in one game shard's account database. The player can still authenticate centrally and play other shards, but the affected shard refuses the handoff.
- A global ban is stored in the login server's central account database. The player cannot complete central authentication, see the shard list, or receive a handoff ticket for any shard.

Game-shard staff can apply local bans in-game:

```text
'localban [durationInMinutes]
'localunban
```

Omitting the duration defaults to 24 hours. A duration of `0` creates a permanent local ban, and timed local bans are capped at 10 years. The older `'ban` and `'unban` commands remain supported as aliases, but they are also shard-local; they never modify the central account database.

The login-server operator applies global bans from the `SERVERTYPE=LOGIN` console. Press `*` to disable secure console mode, then press `B` for GLOBAL BAN or `U` for GLOBAL UNBAN. A global ban asks for the central username and duration in minutes, with `0` meaning permanent. Each action is saved immediately and recorded in `logs/account.log` when console logging is enabled.

A global ban takes effect at the next central login. It does not remotely disconnect a player who is already inside a game shard. The shard owner can separately disconnect that session or apply a local ban.

The login server remains a trust boundary: whoever controls its machine and executable can always refuse authentication, stop the service, or redirect ordinary clients. Command names can make authority clear, but software running on that operator's machine cannot force the operator to provide access. Shard owners should agree on a global-ban policy and use a trusted login-service operator. Shard-local administrators retain independent control of local bans and their local staff credentials.

## Firewall and port forwarding

For the example ports:

| Machine | Port | Needed by |
| --- | ---: | --- |
| Login server | TCP 2593 | Normal UO clients |
| Britannia | TCP 2594 | Relayed players, login-server handoffs, and allowlisted shard admins |
| Test Shard | TCP 2595 | Relayed players, login-server handoffs, and allowlisted shard admins |

The game ports must be reachable by players after relay. Restrict direct administrator access to the owner's static source address at the firewall when the hosting provider supports it. Do not expose file sharing, remote desktop, account files, or the handoff secret merely because the UOX3 game port is public.

## Starting and stopping

Start in this order:

1. Start `Britannia\uox3.exe` from the `Britannia` folder.
2. Start `TestShard\uox3.exe` from the `TestShard` folder.
3. Start `Login\uox3.exe` from the `Login` folder.

Starting shards first lets the login server register a handoff immediately when a player makes a selection. To stop everything, stop the login server first so no new players are sent to shards that are shutting down, then stop each game shard normally and allow it to save.

## Verification checklist

Test all of these before announcing the service:

- A normal player connects to port `2593`, sees both shard names, and enters either selected shard.
- The normal player's central password is not copied into either game shard.
- The staff username is absent from the login server's account files and account directories.
- The GM can connect directly to Britannia on port `2594` from an allowlisted address.
- The same GM connection is refused from an address not listed in `LOCALADMINIPS`.
- An ordinary local account cannot use the direct shard login route.
- Selecting a stopped shard produces a denial instead of a relay to a dead server.
- Every machine has a synchronized clock and the same handoff secret.
- Backups of INIs and account files are stored somewhere the login-server operator cannot access.

## Common errors

`ERROR: DFNs directory ./dfndata/ does not exist` on a login-only process means an older executable is being used or the process is not actually reading a `SERVERTYPE=LOGIN` INI. Update `uox3.exe`, verify the working directory, and confirm the role line is in the `[system]` section.

If the shard list is empty, check `SHARDLIST`, the braces and pipe-separated entries in `shards.ini`, and unique nonzero IDs. If selecting a shard fails, verify the game process is running, the handoff host and port are reachable from the login host, and all processes use the identical secret.

If a direct administrator login is denied, check the game console is using `SERVERTYPE=GAME`, the account is a local GM, and the connecting source address exactly matches `LOCALADMINIPS`. Remember that the public IP seen by the server can differ from the address displayed on the administrator's PC because of NAT or VPN use.
